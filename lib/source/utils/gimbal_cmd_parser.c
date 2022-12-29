#include <gimbal/utils/gimbal_cmd_parser.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/strings/gimbal_string_view.h>
#include <gimbal/meta/types/gimbal_variant.h>

#define GBL_CMD_PARSER_(self) ((GblCmdParser_*)GBL_INSTANCE_PRIVATE(self, GBL_CMD_PARSER_TYPE))

GBL_DECLARE_STRUCT_PRIVATE(GblCmdArg) {
    GblStringRef* pName;
    GblStringRef* pDesc;
};

GBL_DECLARE_STRUCT_PRIVATE(GblCmdParser) {
    GblArrayList    posArgs;
    GblArrayList    optionGroups;
    GblOptionGroup* pMainOptionGroup;

    GblStringRef*   pExecutable;
    GblStringList*  pArgValues;
    GblStringList*  pUnknownOptions;
};

GBL_EXPORT GBL_RESULT GblCmdParser_parse(GblCmdParser* pSelf, GblStringList* pArgs) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pArgs);

    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);

    // 1. Conditionally parse "executable" as first argument
    if(pSelf->firstArgAsExecutable) {
        GBL_CTX_VERIFY(!GblStringList_empty(pArgs),
                       GBL_RESULT_ERROR_INVALID_CMDLINE_ARG,
                       "Expected executable name as first argument, "
                       "but no arguments were provided.");

        pSelf_->pExecutable = GblStringList_popFront(pArgs);
    }

    // 2. Parse main option group
    if(pSelf_->pMainOptionGroup) {
        GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pSelf_->pMainOptionGroup, pArgs));
    }

    // 3. Parse additional option groups
    for(GblSize o = 0; o < GblArrayList_size(&pSelf_->optionGroups); ++o) {
        GblOptionGroup** ppGroup = GblArrayList_at(&pSelf_->optionGroups, o);
        GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(*ppGroup, pArgs));
    }

    // 4. Parse remaining arguments
    GblBool parseAsPositionals = GBL_FALSE;
    GblStringList* pIt = pArgs->ringNode.pNext;

    while(pIt != pArgs) {
        GblStringList* pNext = pIt->ringNode.pNext;
        GblStringView argView = GblStringRef_view(pIt->pData);

        // Check for options or "--" specifier
        if(!parseAsPositionals) {
            // check whether we should consider all remaining args as positional
            if(GblStringView_equals(argView, GBL_STRV("--"))) {
                parseAsPositionals = GBL_TRUE;
                pIt = pNext;
                continue;
            // check whether we've encountered an unknown remaining optional
            } else if(GblStringView_startsWith(argView, GBL_STRV("-"))) {
                // Add option key to unknown option list
                GblDoublyLinkedList_remove(&pIt->listNode);
                GblDoublyLinkedList_pushBack(&pSelf_->pUnknownOptions->listNode, &pIt->listNode);

                // Advance to the next node
                pIt = pNext;
                pNext = pIt->ringNode.pNext;

                // Add option value to unknown option list without removing it (if one exists)
                if(pIt == pArgs) break;

                GblStringList_pushBackRefs(pSelf_->pUnknownOptions,
                                           GblStringRef_acquire(pIt->pData));

            }
        }

        // Move current positional argument from source to internal list
        GblDoublyLinkedList_remove(&pIt->listNode);
        GblDoublyLinkedList_pushBack(&pSelf_->pArgValues->listNode, &pIt->listNode);

        pIt = pNext;
    }

    // 5. Verify positional arguments
    const GblSize actualCount = GblStringList_size(pArgs);
    const GblSize expectedCount = GblArrayList_size(&pSelf_->posArgs);

    GBL_CTX_VERIFY(actualCount == expectedCount ||
                   (pSelf->allowExtraArgs && actualCount > expectedCount),
                   GBL_RESULT_ERROR_INVALID_CMDLINE_ARG,
                   "Expected %u arguments, but %u were provided",
                   expectedCount, actualCount);

    // 6. Verify unknown optional arguments
    GBL_CTX_VERIFY(GblStringList_empty(pSelf_->pUnknownOptions) ||
                   pSelf->allowUnknownOptions,
                   GBL_RESULT_ERROR_INVALID_CMDLINE_ARG,
                   "Unknown option provided: %s", pIt->pData);

    // 7. Free source list, store result code + message, and return
    GBL_CTX_END_BLOCK();
    GblStringList_destroy(pArgs);

    pSelf->parseResult = GBL_CTX_RESULT();
    if(!GBL_RESULT_SUCCESS(pSelf->parseResult)) {
        pSelf->pErrorMsg = GblStringRef_create(GBL_CTX_RECORD().message);
    }

    return pSelf->parseResult;
}

GBL_EXPORT GblCmdParser* GblCmdParser_create(void) {
    return GBL_OBJECT_NEW(GblCmdParser);
}

GBL_EXPORT GblRefCount GblCmdParser_unref(GblCmdParser* pSelf) {
    return GBL_BOX_UNREF(pSelf);
}

GBL_EXPORT GBL_RESULT GblCmdParser_setMainOptionGroup(GblCmdParser* pSelf, GblOptionGroup* pGroup) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pGroup);
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);
    GBL_BOX_UNREF(pSelf_->pMainOptionGroup);
    pSelf_->pMainOptionGroup = pGroup;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblCmdParser_addOptionGroup(GblCmdParser* pSelf, GblOptionGroup* pGroup) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pGroup);
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);
    GBL_CTX_VERIFY_CALL(GblArrayList_pushBack(&pSelf_->optionGroups, &pGroup));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblCmdParser_addPositionalArg(GblCmdParser* pSelf, const char* pName, const char* pDesc) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pName);

    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);
    const GblCmdArg_ arg = {
        GblStringRef_create(pName),
        pDesc? GblStringRef_create(pDesc) : NULL
    };

    GBL_CTX_VERIFY_CALL(GblArrayList_pushBack(&pSelf_->posArgs, &arg));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblCmdParser_clearPositionalArgs(GblCmdParser* pSelf) {
    GBL_CTX_BEGIN(NULL);

    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);
    for(GblSize p = 0; p < GblArrayList_size(&pSelf_->posArgs); ++p) {
        GblCmdArg_* pArg = GblArrayList_at(&pSelf_->posArgs, p);
        GblStringRef_release(pArg->pName);
        GblStringRef_release(pArg->pDesc);
    }
    GBL_CTX_VERIFY_CALL(GblArrayList_clear(&pSelf_->posArgs));

    GBL_CTX_END();
}

GBL_EXPORT GblSize GblCmdParser_positionalArgValueCount(const GblCmdParser* pSelf) {
    GblSize count = 0;
    GBL_CTX_BEGIN(NULL);
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);
    count = GblStringList_size(pSelf_->pArgValues);
    GBL_CTX_END_BLOCK();
    return count;
}

GBL_EXPORT GBL_RESULT GblCmdParser_positonalArgValue(const GblCmdParser* pSelf, GblSize index, GblType toType, void* pData) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_ARG(index < GblCmdParser_positionalArgValueCount(pSelf));
    GBL_CTX_VERIFY_TYPE(toType, GBL_IVARIANT_TYPE);
    GBL_CTX_VERIFY_ARG(GblVariant_canConvert(GBL_STRING_TYPE, toType));
    GBL_CTX_VERIFY_POINTER(pData);

    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);

    GblVariant src = GBL_VARIANT_INIT;
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&src,
                                                      GBL_STRING_TYPE,
                                                      GblStringRef_acquire(GblStringList_at(pSelf_->pArgValues, index))));
    GblVariant dest = GBL_VARIANT_INIT;
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&dest, toType));

    GBL_CTX_VERIFY_CALL(GblVariant_convert(&src, &dest));
    GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(&dest, pData));

    GBL_CTX_END_BLOCK();
    GblVariant_destruct(&src);
    GblVariant_destruct(&dest);
    return GBL_CTX_RESULT();
}

GBL_EXPORT const GblStringList* GblCmdParser_positionalArgValues(const GblCmdParser* pSelf) {
    return GBL_CMD_PARSER_(pSelf)->pArgValues;
}

GBL_EXPORT const GblStringRef* GblCmdParser_executable(const GblCmdParser* pSelf) {
    return GBL_CMD_PARSER_(pSelf)->pExecutable;
}

GBL_EXPORT const GblStringList* GblCmdParser_unknownOptions(const GblCmdParser* pSelf) {
    return GBL_CMD_PARSER_(pSelf)->pUnknownOptions;
}

static GBL_RESULT GblCmdParser_Object_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GblCmdParser* pSelf = GBL_CMD_PARSER(pObject);
    GBL_UNUSED(pSelf, pValue);
    switch(pProp->id) {
    //case GblOptionGroup_Property_Id_name:
    //    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pValue, pProp->valueType, GblStringRef_acquire(GblObject_name(pObject))));
    //    break;
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY, "Reading unhandled property: %s", GblProperty_nameString(pProp));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblCmdParser_Object_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(pProp);
    GblCmdParser* pSelf = GBL_CMD_PARSER(pObject);
    GBL_UNUSED(pSelf, pValue);
    switch(pProp->id) {
    //case GblOptionGroup_Property_Id_name: {
    //    const char* pName = NULL;
    //   GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pValue, &pName));
    //    GblObject_setName(pObject, pName);
    //    break;
    //}
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Writing unhandled property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblCmdParser_Box_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblCmdParser* pSelf = GBL_CMD_PARSER(pBox);
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);

    GblStringRef_release(pSelf->pErrorMsg);

    GblStringRef_release(pSelf_->pExecutable);

    GblStringList_destroy(pSelf_->pArgValues);
    GblStringList_destroy(pSelf_->pUnknownOptions);

    GblOptionGroup_unref(pSelf_->pMainOptionGroup);

    for(GblSize o = 0; o < GblArrayList_size(&pSelf_->optionGroups); ++o) {
        GblOptionGroup** ppGroup = GblArrayList_at(&pSelf_->optionGroups, o);
        GblOptionGroup_unref(*ppGroup);
    }
    GBL_CTX_VERIFY_CALL(GblArrayList_destruct(&pSelf_->optionGroups));

    GBL_CTX_VERIFY_CALL(GblCmdParser_clearPositionalArgs(GBL_CMD_PARSER(pBox)));
    GBL_CTX_VERIFY_CALL(GblArrayList_destruct(&pSelf_->posArgs));

    GBL_INSTANCE_VCALL_DEFAULT(GblObject, base.pFnDestructor, pBox);

    GBL_CTX_END();
}

static GBL_RESULT GblCmdParser_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pInstance);

    GBL_CTX_VERIFY_CALL(GblArrayList_construct(&pSelf_->optionGroups, sizeof(GblOptionGroup*)));
    GBL_CTX_VERIFY_CALL(GblArrayList_construct(&pSelf_->posArgs, sizeof(GblCmdArg_)));

    pSelf_->pArgValues      = GblStringList_createEmpty();
    pSelf_->pUnknownOptions = GblStringList_createEmpty();

    GBL_CTX_END();
}

static GBL_RESULT GblCmdParserClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_UNUSED(pUd);

    GBL_BOX_CLASS(pClass)   ->pFnDestructor  = GblCmdParser_Box_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GblCmdParser_Object_property_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GblCmdParser_Object_setProperty_;

    GBL_CTX_END();
}

GBL_EXPORT GblType GblCmdParser_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);

        type = GblType_registerStatic(GblQuark_internStringStatic("GblCmdParser"),
                                      GBL_OBJECT_TYPE,
                                      &(GblTypeInfo) {
                                          .pFnClassInit        = GblCmdParserClass_init_,
                                          .classSize           = sizeof(GblCmdParserClass),
                                          .pFnInstanceInit     = GblCmdParser_init_,
                                          .instanceSize        = sizeof(GblCmdParser),
                                          .instancePrivateSize = sizeof(GblCmdParser_)
                                      },
                                      GBL_TYPE_FLAGS_NONE);

        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();

    }

    return type;
}
