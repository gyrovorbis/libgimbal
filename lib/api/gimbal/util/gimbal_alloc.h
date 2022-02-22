#ifndef GIMBAL_ALLOC_H
#define GIMBAL_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* SLOT-BASED MEMORY ALLOCATOR FOR ADDING METADATA AND SHIT TO EXISTING TYPES WITH ALSO SUPPORTING EXTENSION
 * - stack-based push/pop API that returns slot indices
 * - hide metadata/metacontext structures from user
 * - hide private variables associated with types from user
 * - make sure object heirarchy space is fixed in size
 * - optionally grow at runtime
 * - push/pop semantics to configure allocator object before actually amallocing
 */


typedef struct GblSlotAllocator {


} GblSlotAllocator;


typedef struct GblMetaAllocation {
    // preset index field or pointer to slot directory
    //

} GblMetaAllocation;

typedef struct GblMetaType {
    GblMetaAllocation baseAlloc;
} GblMetaType;



/*
ALLOC_BEGIN()
SLOT(GBL_META_TYPE, sizeof(GblMetaType));
PUSH(GBL_DISPATCHER_TYPE)
PUSH(PARENTS)
foreach(parent) {
    ADD(sizeof(GblDIspatcher*));
}
POP();
PUSH(VTABLES)
foreach(vtable) {
    ADD(sizeof(GblDIspatcher*));
}
POP();
SLOT(GBL_TYPE_INFO sizeof(GblTypeInfo));
PUSH(BASE_TYPES)
foreach(baseType) ADD(sizeof(GblType));
POP()
SLOT(STRING_NAME, strlen(pstrName));
ALLOC_END();



GblMetaAllocBlock* pBlock = gblAllocBlockNew(&stackSlots);
GblMetaType* pMeta = GblAllocBlockSlot(pBlock, FIXED);
gblAllocSlotOffset();
gblAllocSlotSize();
gblAllocSlotResize();

// make it smart enough to know to update pointers when shit moves around?

// Reserve space or at least keep slot metadata info for size for when shit like a static object becomes dynamic?
*/

#ifdef __cplusplus
}
#endif

#endif // GIMBAL_ALLOC_H
