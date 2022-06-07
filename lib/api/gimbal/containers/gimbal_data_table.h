#ifndef GIMBAL_DATA_TABLE_H
#define GIMBAL_DATA_TABLE_H

#include "../types/gimbal_typedefs.h"
#include "../types/gimbal_quark.h"
#include "../types/gimbal_variant.h"
#include "../types/gimbal_result.h"

#define GBL_DATA_TABLE_SIZE(elements)   (sizeof(GblDataTable) + (sizeof(GblDataTableEntry)*(elements-1)))

#define SELF    GblDataTable** ppSelf
#define CSELF   GblDataTable*const* ppSelf

// make me also take a key
typedef GBL_RESULT (*GblDataTableDestructFn)(void* pValue);

typedef struct GblDataTableEntry {
    GblQuark                    key;
    GblVariant                  value; // using value.type == GBL_INVALID_TYPE to signify userdata
    GblDataTableDestructFn      dtor;
} GblDataTableEntry;

typedef struct GblDataTable {
    uint16_t            count;
    GblDataTableEntry   entries[1]; //variable length, but won't even be allocated unless there's 1
} GblDataTable;

GBL_INLINE GBL_RESULT   GblDataTable_destroy                    (SELF)                                          GBL_NOEXCEPT;

GBL_INLINE uint16_t     GblDataTable_entryCount                 (CSELF)                                         GBL_NOEXCEPT;
GBL_INLINE GblDataTableEntry*
                        GblDataTable_entryFromIndex             (SELF, GblSize index)                           GBL_NOEXCEPT;
GBL_INLINE GblDataTableEntry*
                        GblDataTable_entryFromKeyQuark          (SELF, GblQuark key)                            GBL_NOEXCEPT;
GBL_INLINE GblDataTableEntry*
                        GblDataTable_entryFromKeyString         (SELF, const char* pKey)                        GBL_NOEXCEPT;

GBL_INLINE GblVariant*  GblDataTable_variantFromKeyQuark        (SELF, GblQuark key)                            GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblDataTable_variantFromKeyString       (SELF, const char* pKey)                        GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblDataTable_variantFromIndex           (SELF, uint16_t index)                          GBL_NOEXCEPT;

GBL_INLINE uintptr_t    GblDataTable_valueFromKeyQuark          (CSELF, GblQuark key)                           GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblDataTable_valueFromKeyString         (CSELF, const char* pString)                    GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblDataTable_valueFromIndex             (CSELF, uint16_t index)                         GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblDataTable_variantSetWithKeyQuark     (SELF, GblQuark key, GblVariant* pValue)        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblDataTable_variantSetWithKeyString    (SELF, const char* pKey, GblVariant* pValue)    GBL_NOEXCEPT;
// doesn't resize
GBL_INLINE GBL_RESULT   GblDataTable_variantSetWithIndex        (SELF, uint16_t index, GblVariant* pValue)      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblDataTable_userdataSetWithQuark       (SELF, GblQuark key,
                                                                 uintptr_t value, GblDataTableDestructFn pDtor) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblDataTable_userdataSetWithString      (SELF, const char* pKey,
                                                                 uintptr_t value, GblDataTableDestructFn pDtor) GBL_NOEXCEPT;
// doesn't resize
GBL_INLINE GBL_RESULT   GblDataTable_userdataSetWithIndex       (SELF, uint16_t index,
                                                                 uintptr_t value, GblDataTableDestructFn pDtor) GBL_NOEXCEPT;

//========== IMPL ==========

GBL_INLINE GBL_RESULT GblDataTable_entryDestruct_(GblDataTableEntry* pEntry) {
    if(pEntry->dtor) {
        return pEntry->dtor(pEntry->value.pVoid);
    } else if(GblVariant_type(&pEntry->value) != GBL_INVALID_TYPE) {
        return GblVariant_destruct(&pEntry->value);
    } else return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_RESULT GblDataTable_destroy(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblDataTable* pSelf = *ppSelf;
    if(pSelf) {
        for(uint16_t e = 0; e < pSelf->count; ++e) {
            GBL_API_CALL(GblDataTable_entryDestruct_(&pSelf->entries[e]));
        }
        GBL_API_FREE(pSelf);
        *ppSelf = NULL;
    }
    GBL_API_END();
}

GBL_INLINE uint16_t GblDataTable_entryCount(CSELF) GBL_NOEXCEPT {
    return *ppSelf? (*ppSelf)->count : 0;
}

GBL_INLINE GblDataTableEntry* GblDataTable_entryFromIndex(SELF, GblSize index) GBL_NOEXCEPT {
    GblDataTable* pSelf = *ppSelf;
    if(pSelf) {
        if(index < pSelf->count) {
            return &pSelf->entries[index];
        }
    }
    return NULL;
}

GBL_INLINE GblDataTableEntry* GblDataTable_entryFromKeyQuark(SELF, GblQuark key) GBL_NOEXCEPT {
    GblDataTable* pSelf = *ppSelf;
    if(pSelf) {
        for(uint16_t e = 0; e < pSelf->count; ++e) {
            if(pSelf->entries[e].key == key)
                return &pSelf->entries[e];
        }
    }
    return NULL;
}

GBL_INLINE GblDataTableEntry* GblDataTable_entryFromKeyString(SELF, const char* pKey) GBL_NOEXCEPT {
    GblQuark quark = GBL_QUARK_INVALID;
    if(ppSelf && (quark = GblQuark_tryString(pKey)) != GBL_QUARK_INVALID)
        return GblDataTable_entryFromKeyQuark(ppSelf, quark);
    else
        return NULL;
}


GBL_INLINE GblVariant* GblDataTable_variantFromKeyQuark(SELF, GblQuark key) GBL_NOEXCEPT {
    GblDataTableEntry* pEntry = GblDataTable_entryFromKeyQuark(ppSelf, key);
    return pEntry? &pEntry->value : NULL;
}

GBL_INLINE GblVariant* GblDataTable_variantFromKeyString(SELF, const char* pKey) GBL_NOEXCEPT {
    GblQuark quark = GBL_QUARK_INVALID;
    if(ppSelf && (quark = GblQuark_tryString(pKey)) != GBL_QUARK_INVALID)
        return GblDataTable_variantFromKeyQuark(ppSelf, quark);
    else
        return NULL;
}


GBL_INLINE GblVariant* GblDataTable_variantFromIndex(SELF, uint16_t index) GBL_NOEXCEPT {
    if(ppSelf) {
        GblDataTable* pSelf = *ppSelf;
        if(index < pSelf->count)
            return &pSelf->entries[index].value;
    }
    return NULL;
}

GBL_INLINE uintptr_t GblDataTable_valueFromVariant_(GblVariant* pVariant) GBL_NOEXCEPT {
    uintptr_t value = 0;
    if(pVariant) {
        if(pVariant->type == GBL_INVALID_TYPE)
            value = (uintptr_t)pVariant->pVoid;
        else
            GblVariant_getValuePeek(pVariant, &value);
    }
    return value;
}

GBL_INLINE uintptr_t GblDataTable_valueFromKeyQuark(CSELF, GblQuark key) GBL_NOEXCEPT {
    GblVariant* pVariant = GblDataTable_variantFromKeyQuark((GblDataTable**)ppSelf, key);
    return GblDataTable_valueFromVariant_(pVariant);
}

GBL_INLINE uintptr_t GblDataTable_valueFromKeyString(CSELF, const char* pString) GBL_NOEXCEPT {
    GblQuark quark = GBL_QUARK_INVALID;
    if(ppSelf && (quark = GblQuark_tryString(pString)) != GBL_QUARK_INVALID) {
        return GblDataTable_valueFromKeyQuark(ppSelf, quark);
    }
    return 0;
}

GBL_INLINE uintptr_t GblDataTable_valueFromIndex(CSELF, uint16_t index) GBL_NOEXCEPT {
    GblVariant* pVariant = GblDataTable_variantFromIndex((GblDataTable**)ppSelf, index);
    return GblDataTable_valueFromVariant_(pVariant);
}

GBL_INLINE GblDataTableEntry* GblDataTable_entryAdd_(SELF, GblQuark key) {
    GblDataTableEntry* pEntry = NULL;
    GBL_API_BEGIN(NULL);
    if(!*ppSelf) {
        *ppSelf = (GblDataTable*)GBL_API_MALLOC(gblAlignedAllocSize(GBL_DATA_TABLE_SIZE(1)), GBL_ALIGNOF(GBL_MAX_ALIGN_T), GblQuark_toString(key));
        (*ppSelf)->count = 0;
    }  else {
        *ppSelf = (GblDataTable*)GBL_API_REALLOC(*ppSelf, GBL_DATA_TABLE_SIZE((*ppSelf)->count+1));
    }
    pEntry = &(*ppSelf)->entries[(*ppSelf)->count];
    memset(pEntry, 0, sizeof(GblDataTableEntry));
    pEntry->key = key;
    (*ppSelf)->count++;
    GBL_API_END_BLOCK();
    return pEntry;
}

GBL_INLINE GBL_RESULT GblDataTable_entrySetVariant_(GblDataTableEntry* pEntry, GblVariant* pVariant) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    if(pEntry->dtor && GblVariant_type(&pEntry->value) != GblVariant_type(pVariant)) {
        GBL_API_CALL(pEntry->dtor(pVariant->pVoid));
        pEntry->dtor = NULL;
        GBL_API_CALL(GblVariant_constructCopy(&pEntry->value, pVariant));
    } else {
        GBL_API_CALL(GblVariant_setCopy(&pEntry->value, pVariant));
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblDataTable_variantSetWithKeyQuark(SELF, GblQuark key, GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblDataTableEntry* pEntry = GblDataTable_entryFromKeyQuark(ppSelf, key);
    if(!pEntry) {
        pEntry = GblDataTable_entryAdd_(ppSelf, key);
        GBL_API_CALL(GblVariant_constructCopy(&pEntry->value, pValue));
    } else {
        GBL_API_CALL(GblDataTable_entrySetVariant_(pEntry, pValue));
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblDataTable_variantSetWithKeyString(SELF, const char* pKey, GblVariant* pValue) GBL_NOEXCEPT {
    return GblDataTable_variantSetWithKeyQuark(ppSelf, GblQuark_fromString(pKey), pValue);
}

GBL_INLINE GBL_RESULT GblDataTable_variantSetWithIndex(SELF, uint16_t index, GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblDataTableEntry* pEntry = GblDataTable_entryFromIndex(ppSelf, index);
    GBL_API_VERIFY(pEntry,
                   GBL_RESULT_ERROR_OUT_OF_RANGE,
                   "[GblDataTable]: Failed to set value at index: %u", index);
    GBL_API_CALL(GblDataTable_entrySetVariant_(pEntry, pValue));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblDataTable_entrySetUserdata_(GblDataTableEntry* pEntry, uintptr_t value, GblDataTableDestructFn pDtor) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    if(pEntry->dtor) {
       GBL_API_CALL(pEntry->dtor(pEntry));
   } else if(GblVariant_type(&pEntry->value) != GBL_INVALID_TYPE) {
       GBL_API_CALL(GblVariant_destruct(&pEntry->value));
   }
   pEntry->value.pVoid = (void*)value;
   pEntry->value.type = GBL_INVALID_TYPE;
   pEntry->dtor = pDtor;
   GBL_API_END();
}

GBL_INLINE GBL_RESULT GblDataTable_userdataSetWithQuark(SELF, GblQuark key, uintptr_t value, GblDataTableDestructFn pDtor) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblDataTableEntry* pEntry = GblDataTable_entryFromKeyQuark(ppSelf, key);
    if(!pEntry) {
        pEntry = GblDataTable_entryAdd_(ppSelf, key);
    }
    GBL_API_CALL(GblDataTable_entrySetUserdata_(pEntry, value, pDtor));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblDataTable_userdataSetWithString(SELF, const char* pKey, uintptr_t value, GblDataTableDestructFn pDtor) GBL_NOEXCEPT {
    return GblDataTable_userdataSetWithQuark(ppSelf, GblQuark_fromString(pKey), value, pDtor);
}
// doesn't insert
GBL_INLINE GBL_RESULT GblDataTable_userdataSetWithIndex(SELF, uint16_t index, uintptr_t value, GblDataTableDestructFn pDtor) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblDataTableEntry* pEntry = GblDataTable_entryFromIndex(ppSelf, index);
    GBL_API_VERIFY(pEntry,
                   GBL_RESULT_ERROR_OUT_OF_RANGE,
                   "[GblDataTable]: Failed to set value at index: %u", index);
    GBL_API_CALL(GblDataTable_entrySetUserdata_(pEntry, value, pDtor));
    GBL_API_END();
}




#undef CSELF
#undef SELF

#endif // GIMBAL_DATA_TABLE_HPP
