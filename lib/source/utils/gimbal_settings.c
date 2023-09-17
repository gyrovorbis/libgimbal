#include <gimbal/utils/gimbal_settings.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/containers/gimbal_nary_tree.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/meta/types/gimbal_variant.h>

#define GBL_SETTINGS_HASHSET_SIZE_DEFAULT_  32
#define GBL_SETTINGS_(self)                 (GBL_PRIVATE(GblSettings, self))

typedef union GblSettingsNode {
    GblNaryTreeNode treeNode;
} GblSettingsNode;

GBL_DECLARE_STRUCT(GblSettings_) {
    GblHashSet      hashSet;
    GblStringBuffer scope;
};

GBL_DECLARE_STRUCT(GblSettingsEntry_) {
    GblStringRef*   pKey;
    GblVariant      value;
};

static GblHash GblSettings_hashSet_hasher_(const GblHashSet* pSet, const void* pEntry) {
    const GblSettingsEntry_* pEntry_ = pEntry;
    return gblHash(pEntry_->pKey, strlen(pEntry_->pKey));
}

static GblBool GblSettings_hashSet_comparator_(const GblHashSet* pSet, const void* pEntry1, const void* pEntry2) {
    const GblSettingsEntry_* pEntry1_ = pEntry1;
    const GblSettingsEntry_* pEntry2_ = pEntry2;
    return strcmp(pEntry1_->pKey, pEntry2_->pKey) == 0;
}

static void GblSettings_hashSet_destructor_(const GblHashSet* pSet, void* pEntry) {
    GblSettingsEntry_* pEntry_ = pEntry;
    GblStringRef_unref(pEntry_->pKey);
    GblVariant_destruct(&pEntry_->value);
}

GBL_EXPORT GblSettings* GblSettings_create(void) {
    return GBL_NEW(GblSettings);
}

GBL_EXPORT GblSettings* GblSettings_ref(const GblSettings* pSelf) {
    return GBL_SETTINGS(GBL_REF(pSelf));
}

GBL_EXPORT GblRefCount GblSettings_unref(GblSettings* pSelf) {
    return GBL_UNREF(pSelf);
}

GBL_EXPORT const char* GblSettings_scope(const GblSettings* pSelf) {
    return GblStringBuffer_cString(&GBL_SETTINGS_(pSelf)->scope);
}

GBL_EXPORT const char* GblSettings_pushScope(GblSettings* pSelf, const char* pKey) {
    GblSettings_* pSelf_ = GBL_SETTINGS_(pSelf);

    if(!GblStringBuffer_empty(&pSelf_->scope))
        GblStringBuffer_appendPrintf(&pSelf_->scope, "/%s", pKey);
    else
        GblStringBuffer_append(&pSelf_->scope, pKey);

    return GblStringBuffer_cString(&pSelf_->scope);
}

GBL_EXPORT const char* GblSettings_pop(GblSettings* pSelf) {
    GblSettings_* pSelf_ = GBL_SETTINGS_(pSelf);

    const GblStringView view = {
        .pData = GblStringBuffer_cString(&pSelf_->scope),
        .nullTerminated = GBL_TRUE,
        .length = GblStringBuffer_length(&pSelf_->scope)
    };

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY(!GblStringView_empty(view),
                   GBL_RESULT_ERROR_OUT_OF_RANGE,
                   "Cannot pop from empty scope!");

    const size_t cutOff = GblStringView_findLastOf(view, "/");

    if(cutOff != GBL_STRING_VIEW_NPOS)
        GblStringBuffer_shrink(&pSelf_->scope, view.length - cutOff);
    else
        GblStringBuffer_clear(&pSelf_->scope);

    GBL_CTX_END_BLOCK();
    return GblStringBuffer_cString(&pSelf_->scope);
}

static GBL_RESULT GblSettings_read_(GblSettings* pSelf, const char* pKey, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblBool            absolute = GBL_FALSE;
    GblSettingsEntry_* pEntry   = NULL;

    // If not absolute path, append key to scope
    if(pKey[0] != '/')
        pKey = GblSettings_pushScope(pSelf, pKey);
    else {  // else use key directly, skipping '/'
        ++pKey;
        absolute = GBL_TRUE;
    }

    // Grab entry
    pEntry = GblHashSet_get(&GBL_SETTINGS_(pSelf)->hashSet, &pKey);

    // Copy value if we have it, set nil otherwise
    if(pEntry)
        GblVariant_setCopy(pValue, &pEntry->value);
    else
        GblVariant_setNil(pValue);


    // Pop key from scope scope if not absolute key
    if(!absolute)
        GblSettings_pop(pSelf);

    GBL_CTX_END();
}

static GBL_RESULT GblSettings_write_(GblSettings* pSelf, const char* pKey, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblBool       absolute = GBL_FALSE;
    GblSettings_* pSelf_   = GBL_SETTINGS_(pSelf);

    // If not absolute path, append key to scope
    if(pKey[0] != '/')
        pKey = GblSettings_pushScope(pSelf, pKey);
    else {  // Using absolute path, skip '/'
        ++pKey;
        absolute = GBL_TRUE;
    }

    // Check if we're removing an entry (setting it equal to "nil")
    if(GblVariant_isNil(pValue)) {
        const GblBool erased = GblHashSet_erase(&pSelf_->hashSet, &pKey);
        GBL_CTX_VERIFY(erased,
                       GBL_RESULT_ERROR_INTERNAL,
                       "Erase failed for setting key: [%s]",
                       pKey);

        // Emit erased signal
        GBL_EMIT(pSelf, "erased", pKey);

    } else { // Setting existing or adding new entry

        // Grab entry, if exists
        GblSettingsEntry_* pEntry = GblHashSet_get(&pSelf_->hashSet, &pKey);
        GblBool added = GBL_FALSE;

        // If it doesn't exist, emplace a new one
        if(!pEntry) {
            pEntry = GblHashSet_emplace(&pSelf_->hashSet, &pKey);
            GBL_CTX_VERIFY(pEntry,
                           GBL_RESULT_ERROR_INTERNAL,
                           "Failed to find entry for key [%s]",
                           pKey);

            // Set new Entry fields
            pEntry->pKey = GblStringRef_create(pKey);
            GblVariant_constructMove(&pEntry->value, pValue);
            GBL_EMIT(pSelf, "added", pKey);

        // If entry exists + value has changed
        } else if(!GblVariant_equals(&pEntry->value, pValue)) {
            GblVariant_setMove(&pEntry->value, pValue);
            GBL_EMIT(pSelf, "changed", pKey);
        }
    }

    // If relative path, pop key from scope name to restore previous
    if(!absolute)
        GblSettings_pop(pSelf);

    GBL_CTX_END();
}

static GBL_RESULT GblSettings_Object_property(const GblObject* pObject, const GblProperty* pProperty, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_END();
}

static GBL_RESULT GblSettings_Object_setProperty(GblObject* pObject, const GblProperty* pProperty, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_END();
}

static GBL_RESULT GblSettings_Box_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblSettings* pSelf   = GBL_SETTINGS(pBox);
    GblSettings_* pSelf_ = GBL_SETTINGS_(pSelf);

    // Destruct public heap data
    GblStringRef_unref(pSelf->pPath);
    GblStringRef_unref(pSelf->pAppName);
    GblStringRef_unref(pSelf->pOrgName);

    // Destruct private data
    GBL_CTX_CALL(GblHashSet_destruct(&pSelf_->hashSet));
    GBL_CTX_CALL(GblStringBuffer_destruct(&pSelf_->scope));

    // Call parent's destructor
    GBL_VCALL_DEFAULT(GblObject, base.pFnDestructor, pBox);

    GBL_CTX_END();
}

static GBL_RESULT GblSettings_init_(GblInstance* pInstance) {
    GBL_CTX_BEGIN(NULL);

    GblSettings* pSelf   = GBL_SETTINGS(pInstance);
    GblSettings_* pSelf_ = GBL_SETTINGS_(pInstance);

    GBL_CTX_CALL(GblHashSet_construct(&pSelf_->hashSet,
                                      sizeof(GblSettingsEntry_),
                                      GblSettings_hashSet_hasher_,
                                      GblSettings_hashSet_comparator_,
                                      GblSettings_hashSet_destructor_,
                                      GBL_SETTINGS_HASHSET_SIZE_DEFAULT_,
                                      NULL,
                                      pSelf));

    GBL_CTX_CALL(GblStringBuffer_construct(&pSelf_->scope));

    GBL_CTX_END();
}

static GBL_RESULT GblSettingsClass_init_(GblClass* pClass, const void* pUd) {
    GBL_CTX_BEGIN(NULL);

    GBL_BOX_CLASS(pClass)     ->pFnDestructor  = GblSettings_Box_destructor_;
    GBL_OBJECT_CLASS(pClass)  ->pFnProperty    = GblSettings_Object_property;
    GBL_OBJECT_CLASS(pClass)  ->pFnSetProperty = GblSettings_Object_setProperty;
    GBL_SETTINGS_CLASS(pClass)->pFnRead        = GblSettings_read_;
    GBL_SETTINGS_CLASS(pClass)->pFnWrite       = GblSettings_write_;

    GBL_CTX_END();
}

GBL_EXPORT GblType GblSettings_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .pFnClassInit        = GblSettingsClass_init_,
        .classSize           = sizeof(GblSettingsClass),
        .pFnInstanceInit     = GblSettings_init_,
        .instanceSize        = sizeof(GblSettings),
        .instancePrivateSize = sizeof(GblSettings_)
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        type = GblType_register(GblQuark_internStatic("GblSettings"),
                                GBL_OBJECT_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}


