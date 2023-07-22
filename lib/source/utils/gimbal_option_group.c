#include <gimbal/utils/gimbal_option_group.h>
#include <gimbal/meta/signals/gimbal_signal.h>
#include <gimbal/meta/signals/gimbal_marshal.h>
#include <math.h>

GblType argToGblType_(GBL_OPTION_TYPE type) {
    switch(type) {
    default:
    case GBL_OPTION_TYPE_STRING:  return GBL_STRING_TYPE;
    case GBL_OPTION_TYPE_BOOL:    return GBL_BOOL_TYPE;
    case GBL_OPTION_TYPE_UINT8:   return GBL_UINT8_TYPE;
    case GBL_OPTION_TYPE_CHAR:    return GBL_CHAR_TYPE;
    case GBL_OPTION_TYPE_UINT16:  return GBL_UINT16_TYPE;
    case GBL_OPTION_TYPE_INT16:   return GBL_INT16_TYPE;
    case GBL_OPTION_TYPE_UINT32:  return GBL_UINT32_TYPE;
    case GBL_OPTION_TYPE_INT32:   return GBL_INT32_TYPE;
    case GBL_OPTION_TYPE_UINT64:  return GBL_UINT64_TYPE;
    case GBL_OPTION_TYPE_INT64:   return GBL_INT64_TYPE;
    case GBL_OPTION_TYPE_FLOAT:   return GBL_FLOAT_TYPE;
    case GBL_OPTION_TYPE_DOUBLE:  return GBL_DOUBLE_TYPE;
    }
}

GBL_EXPORT GblOptionGroup* GblOptionGroup_create(const char*      pName,
                                                 const char*      pPrefix,
                                                 const GblOption* pOptions) {
    return GBL_OBJECT_NEW(GblOptionGroup,
                          "name",    pName,
                          "prefix",  pPrefix,
                          "options", pOptions);
}

GBL_EXPORT GblRefCount GblOptionGroup_unref(GblOptionGroup* pSelf) {
    return GBL_UNREF(pSelf);
}

GBL_EXPORT GBL_RESULT (GblOptionGroup_parse)(GblOptionGroup* pSelf,
                                             GblStringList*  pList,
                                             GblBool         prefixOnly) {
    GBL_CTX_BEGIN(NULL);

    GblOptionGroupClass* pClass = GBL_OPTION_GROUP_GET_CLASS(pSelf);
    pSelf->parseResult = GBL_RESULT_SUCCESS;

    // Emit signal for preprocessing hook
    GBL_CTX_VERIFY_CALL(GblSignal_emit(GBL_INSTANCE(pSelf), "parsePrePass", pList));
    GBL_CTX_RECORD_SET(pSelf->parseResult);

    GblStringList* pIt = pList->ringNode.pNext;
    while(pIt != pList) {
        // remove leading '-' characters
        GblStringView key = GblStringRef_view(pIt->pData);
        const size_t  begin = GblStringView_findFirstNotOf(key, GBL_STRV("-"), 0);
        // check whether we even have an option
        if(begin == 0 || (begin > 2 && begin != GBL_STRING_VIEW_NPOS)) {
            pIt = pIt->ringNode.pNext;
            continue;
        // Stop parsing upon reaching a standalone "--"
        } else if(begin == GBL_STRING_VIEW_NPOS && key.length == 2) {
            GBL_CTX_DONE();
        }
        key = GblStringView_removePrefix(key, begin);

        // remove 'prefix-' characters (requires --prefix-option with two hyphens)
        if(begin == 2 && GblStringView_startsWith(key, GBL_STRV(pSelf->pPrefix))) {
            if(key.pData[GblStringRef_length(pSelf->pPrefix)] == '-') {
                key = GblStringView_removePrefix(key, GblStringRef_length(pSelf->pPrefix)+1);
                // don't support short prefixed commands like "-prefix-h"
                if(key.length == 1) {
                    pIt = pIt->ringNode.pNext;
                    continue;
                }
            }
        // skip option if it lacked a prefix while prefixes are required
        } else if(prefixOnly) {
            pIt = pIt->ringNode.pNext;
            continue;
        }

        GblBool noValue  = GBL_FALSE;
        GblStringList* pNext = pIt->ringNode.pNext;
        GblStringView value = { 0 };

        size_t  equalsPos = GblStringView_find(key, GBL_STRV("="), 0);
        // Check for '=' sign, value is part of option string
        if(equalsPos != GBL_STRING_VIEW_NPOS) {
            GBL_CTX_VERIFY(key.length > equalsPos,
                           GBL_RESULT_ERROR_INVALID_CMDLINE_ARG,
                           "Option [%s] provided using '= with no value!",
                           key.pData);
            key.length = equalsPos;
            value = GblStringView_fromString(key.pData + equalsPos + 1);
            noValue = GBL_TRUE;
        } else {
            // Check to see that we have a following value argument
            if(pNext != pList) {
                value = GblStringRef_view(pNext->pData);
                // ignore the next "value" if it's actually an option
                if(GblStringView_startsWith(value, GBL_STRV("-")) ||
                   GblStringView_startsWith(value, GBL_STRV("--")))
                {
                    // Don't consider a negative number an option
                    double testNum = GblStringView_toDouble(value);
                    if(isnan(testNum) || testNum >= 0.0) {
                        value = GblStringView_fromEmpty();
                        noValue = GBL_TRUE;
                    }
                }

            } else {
                // EOF, no following value
                value = GblStringView_fromEmpty();
                noValue = GBL_TRUE;
            }
        }

        // Skip short options with long names (-o or --option not -option)
        if(begin == 1 && key.length != 1) {
            pIt = pNext;
        } else {
            size_t  consumed = 0;
            GBL_CTX_VERIFY_CALL(pClass->pFnTry(pSelf, key, value, &consumed));
            if(noValue && consumed == 2) --consumed;
            if(consumed) {
                // Remove all consumed arguments from input list and store in our own list
                while(consumed--) {
                    pNext = pIt->ringNode.pNext;
                    GblDoublyLinkedList_remove(&pIt->listNode);
                    --pList->size;
                    GblDoublyLinkedList_pushBack(&pSelf->pParsedArgs->listNode, &pIt->listNode);
                    pIt = pNext;
                }
            } else pIt = pNext;
        }
    }

    // Emit signal for postprocessing hook
    GBL_CTX_VERIFY_CALL(GblSignal_emit(GBL_INSTANCE(pSelf), "parsePostPass", pList));
    GBL_CTX_RECORD_SET(pSelf->parseResult);
    GBL_CTX_END_BLOCK();
    // Emit signal for parsing error
    if(!GBL_RESULT_SUCCESS(GBL_CTX_RESULT())) {
        GblSignal_emit(GBL_INSTANCE(pSelf), "parseError", GBL_CTX_RESULT());
    }
    pSelf->parseResult = GBL_CTX_RESULT();
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblOptionGroup_try_(GblOptionGroup* pSelf, GblStringView key, GblStringView value, size_t * pConsumed) {
    GBL_CTX_BEGIN(NULL);

    GblVariant v1 = GBL_VARIANT_INIT;
    GblVariant v2 = GBL_VARIANT_INIT;
    char staging[256];

    *pConsumed = 0;
    // search through all options, looking for a match
    for(size_t  o = 0; o < pSelf->optionCount; ++o) {
        const GblOption* pOption = &pSelf->pOptions[o];

        // Ensure that there's either a long or short name matching
        if((pOption->pLongName && GblStringView_equals(key, GBL_STRV(pOption->pLongName))) ||
           (pOption->shortName && (key.length == 1 && key.pData[0] == pOption->shortName)))
        {

            // Ensure there's a value provided if the option requires one
            GBL_CTX_VERIFY(!(!(pOption->type == GBL_OPTION_TYPE_BOOL &&
                               (pOption->flags & GBL_OPTION_FLAG_BOOL_NO_VALUE))
                             && GblStringView_empty(value)),
                           GBL_RESULT_ERROR_INVALID_ARG,
                           "No value given for option. [%s]",
                           GblStringView_toCString(key, staging, sizeof(staging)));

            // Handle booleans with no arguments
            if(pOption->type == GBL_OPTION_TYPE_BOOL && GblStringView_empty(value)) {
                *(GblBool*)pOption->pOutput.pData =
                        (pOption->flags & GBL_OPTION_FLAG_BOOL_INVERTED)?
                            GBL_FALSE : GBL_TRUE;
                *pConsumed = 1;
                GBL_CTX_DONE();

            // Handle callbacks
            } else if(pOption->type == GBL_OPTION_TYPE_CALLBACK) {
                GblBool consumed = GBL_FALSE;
                *pConsumed = 1;
                GBL_CTX_VERIFY_POINTER(pOption->pOutput.pFn);
                GBL_CTX_VERIFY_CALL(pOption->pOutput.pFn(pSelf, pOption, value, &consumed));
                if(consumed) ++(*pConsumed);
            // Handle strings separately so that we can control their ownership
            } else if(pOption->type == GBL_OPTION_TYPE_STRING) {
                *pConsumed = 2;
                *(const char**)pOption->pOutput.pData = value.pData;
            } else {
                *pConsumed = 2;
                GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v1,
                                                                  GBL_STRING_TYPE,
                                                                  GblStringRef_createFromView(value)));
                GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v2,
                                                                argToGblType_(pOption->type)));

                GBL_CTX_VERIFY_CALL(GblVariant_convert(&v1, &v2));
                GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(&v2, pOption->pOutput.pData));
                // Have to flip an inverted bool
                if(pOption->type == GBL_OPTION_TYPE_BOOL && (pOption->flags & GBL_OPTION_FLAG_BOOL_INVERTED)) {
                    *(GblBool*)pOption->pOutput.pData =
                            (*(GblBool*)pOption->pOutput.pData == GBL_TRUE)? GBL_FALSE : GBL_TRUE;
                }
                GBL_CTX_CALL(GblVariant_destruct(&v1));
                GBL_CTX_CALL(GblVariant_destruct(&v2));
            }
            break;
        }
    }

    GBL_CTX_END_BLOCK();
    GBL_CTX_CALL(GblVariant_destruct(&v1));
    GBL_CTX_CALL(GblVariant_destruct(&v2));
    return GBL_CTX_RESULT();

}

static GBL_RESULT GblOptionGroup_Object_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GblOptionGroup* pSelf = GBL_OPTION_GROUP(pObject);
    switch(pProp->id) {
    //case GblOptionGroup_Property_Id_name:
    //    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pValue, pProp->valueType, GblStringRef_acquire(GblObject_name(pObject))));
    //    break;
    case GblOptionGroup_Property_Id_prefix:
        GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pValue, pProp->valueType, GblStringRef_acquire(pSelf->pPrefix)));
        break;
    case GblOptionGroup_Property_Id_options:
        GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pValue, pProp->valueType, pSelf->pOptions));
        break;
    case GblOptionGroup_Property_Id_version:
        GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pValue, pProp->valueType, pSelf->version));
        break;
    case GblOptionGroup_Property_Id_summary:
        GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pValue, pProp->valueType, GblStringRef_acquire(pSelf->pSummary)));
        break;
    case GblOptionGroup_Property_Id_description:
        GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pValue, pProp->valueType, GblStringRef_acquire(pSelf->pDescription)));
        break;
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY, "Reading unhandled property: %s", GblProperty_nameString(pProp));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblOptionGroup_Object_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(pProp);
    GblOptionGroup* pSelf = GBL_OPTION_GROUP(pObject);

    switch(pProp->id) {
    //case GblOptionGroup_Property_Id_name: {
    //    const char* pName = NULL;
    //   GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pValue, &pName));
    //    GblObject_setName(pObject, pName);
    //    break;
    //}
    case GblOptionGroup_Property_Id_prefix:
        GBL_CTX_VERIFY_CALL(GblVariant_getValueMove(pValue, &pSelf->pPrefix));
        break;
    case GblOptionGroup_Property_Id_options: {
        GBL_CTX_FREE(pSelf->pOptions);
        pSelf->optionCount = 0;
        const GblOption* pOpt;
        GBL_CTX_VERIFY_CALL(GblVariant_getValueMove(pValue, &pOpt));
        const GblOption* pIt = pOpt;
        while(pIt->pLongName || pIt->shortName) {
            ++pSelf->optionCount;
            ++pIt;
        }
        GblOption* pNew = GBL_CTX_NEW(GblOption, pSelf->optionCount);
        memcpy(pNew, pOpt, sizeof(GblOption) * pSelf->optionCount);
        pSelf->pOptions = pNew;
        break;
    }
    case GblOptionGroup_Property_Id_version:
        GBL_CTX_VERIFY_CALL(GblVariant_getValueMove(pValue, &pSelf->version));
        break;
    case GblOptionGroup_Property_Id_summary:
        GBL_CTX_VERIFY_CALL(GblVariant_getValueMove(pValue, &pSelf->pSummary));
        break;
    case GblOptionGroup_Property_Id_description:
        GBL_CTX_VERIFY_CALL(GblVariant_getValueMove(pValue, &pSelf->pDescription));
        break;
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Writing unhandled property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblOptionGroup_Box_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblOptionGroup* pGroup = GBL_OPTION_GROUP(pBox);
    GBL_CTX_FREE(pGroup->pOptions);
    GblStringRef_release(pGroup->pPrefix);
    GblStringRef_release(pGroup->pDescription);
    GblStringRef_release(pGroup->pSummary);
    GblStringList_destroy(pGroup->pParsedArgs);

    GBL_INSTANCE_VCALL_DEFAULT(GblObject, base.pFnDestructor, pBox);

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblOptionGroup_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_OPTION_GROUP(pInstance)->pParsedArgs = GblStringList_createEmpty();
    GBL_CTX_END();
}

static GBL_RESULT GblOptionGroupClass_init_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GBL_UNUSED(pData);

    if(!GblType_classRefCount(GBL_OPTION_GROUP_TYPE)) {
        GBL_PROPERTIES_REGISTER(GblOptionGroup);

        GblSignal_install(GBL_OPTION_GROUP_TYPE,
                          "parsePrePass",
                          GblMarshal_CClosure_VOID__INSTANCE_POINTER,
                          1,
                          GBL_POINTER_TYPE);

        GblSignal_install(GBL_OPTION_GROUP_TYPE,
                          "parsePostPass",
                          GblMarshal_CClosure_VOID__INSTANCE_POINTER,
                          1,
                          GBL_POINTER_TYPE);

        GblSignal_install(GBL_OPTION_GROUP_TYPE,
                          "parseError",
                          GblMarshal_CClosure_VOID__INSTANCE_ENUM,
                          1,
                          GBL_ENUM_TYPE);
  /*
        GblMarshalFn marshals[] = {
            GblMarshal_CClosure_VOID__INSTANCE_POINTER,
            GblMarshal_CClosure_VOID__INSTANCE_POINTER,
            GblMarshal_CClosure_VOID__INSTANCE_ENUM
        };

        GBL_SIGNALS_INSTALL(GblOptionGroup, marshals)
    */
    }

    GBL_OPTION_GROUP_CLASS(pClass)->pFnTry   = GblOptionGroup_try_;
    GBL_BOX_CLASS(pClass)->pFnDestructor     = GblOptionGroup_Box_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GblOptionGroup_Object_property_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GblOptionGroup_Object_setProperty_;
    GBL_CTX_END();
}

static GBL_RESULT GblOptionGroupClass_final_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GBL_UNUSED(pClass, pData);

    if(!GblType_classRefCount(GBL_OPTION_GROUP_TYPE)) {
        GblSignal_uninstall(GBL_OPTION_GROUP_TYPE, "parsePrePass");
        GblSignal_uninstall(GBL_OPTION_GROUP_TYPE, "parsePostPass");
        GblSignal_uninstall(GBL_OPTION_GROUP_TYPE, "parseError");
    }

    GBL_CTX_END();
}

GBL_EXPORT GblType GblOptionGroup_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize       = sizeof(GblOptionGroupClass),
        .pFnClassInit    = GblOptionGroupClass_init_,
        .pFnClassFinal   = GblOptionGroupClass_final_,
        .instanceSize    = sizeof(GblOptionGroup),
        .pFnInstanceInit = GblOptionGroup_init_
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("GblOptionGroup"),
                                      GBL_OBJECT_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}
