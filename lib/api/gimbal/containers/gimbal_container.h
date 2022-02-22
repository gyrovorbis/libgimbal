#ifndef GIMBAL_CONTAINER_H
#define GIMBAL_CONTAINER_H

#ifdef __cplusplus
extern "C" {
#endif

//#include "../gimbal_types.h"
//#include "../gimbal_api.h"



/*A. Sequence containers:
 * 1) vector
 * 2) forward list
 *
 * B. Associative containers:
 * 1) map
 * 2) hash
 *
 */

// JUST FOCUS ON VECTOR + MAP FOR NOW, UNIFY STRING + VECTOR



/*
#define GBL_META_OBJECT_VECTOR_TABLE (                                                               \
        ( GblVector, Vector, "Dynamic Array Container"),     \
        (                                              
            (gblVectorClear, clear, (void)


\
            (GBL_META_TYPE_VTABLE_TO,           0x0,    To,          "To"),                        \
            (GBL_META_TYPE_VTABLE_COMPARE,      0x1,    Compare,     "Compare"),                   \
            (GBL_META_TYPE_VTABLE_DESTRUCTOR,   0x2,    Destructor,  "Destructor"),                     \
            (GBL_META_TYPE_VTABLE_COUNT,        0x3,    BuiltinCount, "Builtin virtual method count")  \
        )                                                                                          \
    )
*/


// Do all the convenience shit for GblVariant too

// vararg initializer for create/assign?
//GBL_VECTOR_FOREACH

/*
//overload so you can do
GblVector vec = GBL_API_CALL_INLINE(VECTOR_CREATE [, GblContext hCtx, GblSize elementSize, GblSize structSize, const void* pInitialData, GblSize elemenCount));


*/

/* STRING SHIT THAT CAN GO HERE:
 * context
 * isEmpty()
 * isStack()
 * clear()
 * take()/give()
 * stackSize()
 * capacity()
 * resize()/reserve()
 * concatenate()
*/
// set comparator for for find?



#if 0


typedef struct GblAssociativeContainer {
    GblContainer        containerBase;
    GblSize             keySize;

} GblAssociativeContainer;

GBL_RESULT gblMapConstruct(GblMap* pMap, GblContext* pCtx, GblSize keySize, GblSize valueSize, Comparator*);
GBL_RESULT gblMapDestruct(GblMap* pMap);
GBL_RESULT gblMapAt(GblMap* pMap, void* pKey, void** ppEntry);
GBL_RESULT gblMapNext(GblMap* pMap, void* pKeyCurrent, void* pKeyNext);
GBL_RESULT gblMapInsert(GblMap* pMap, const void* pKey, const void* pValue);
GBL_RESULT gblMapErase(GblMap* pMap, const void* pKey);



#endif


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_CONTAINER_H
