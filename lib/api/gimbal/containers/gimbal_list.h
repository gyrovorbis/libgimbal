#ifndef GIMBAL_LIST_H
#define GIMBAL_LIST_H

#include "../types/gimbal_typedefs.h"

GBL_DECLS_BEGIN

#define GBL_LIST_NODE(nodePtr)  ((GblListNode*)nodePtr)
#define GBL_DLIST_NODE(nodePtr) ((GblDListNode*)nodePtr)

typedef struct GblListNode {
    struct GblListNode* pNext;
} GblListNode;

typedef struct GblPointerListNode {
    GblListNode         base;
    void*               pPtr;
} GblPointerListNode;

typedef struct GblDListNode {
    union {
        struct GblDListNode*   pNext;
        GblListNode            base;
    };
   struct GblDListNode*   pPrev;
} GblDListNode;


typedef struct GblPointerDListNode {
    GblDListNode        base;
    void*               pPtr;
} GblPointerDListNode;

GBL_INLINE GBL_RESULT   GblListNode_insertAfter         (GblListNode* pNode, GblContext* pCtx);
GBL_INLINE GBL_RESULT   GblListNode_removeAfter         (GblListNode* pNode, GblContext* pCtx);
GBL_INLINE GblSize      GblListNode_countAfter          (GblListNode* pNode);

GBL_INLINE GBL_RESULT  GblPointerListNode_insertAfter   (GblPointerListNode* pNode, void* pPtr);

GBL_INLINE GBL_RESULT GblDListNode_insertBefore         (GblDListNode* pNode);
GBL_INLINE GBL_RESULT GblDListNode_removeBefore         (GblDListNode* pNode);
GBL_INLINE GBL_RESULT GblDListNode_count                (GblDListNode* pNode);

GBL_INLINE GBL_RESULT GblPointerDListNode_insertAfter   (GblPointerDListNode* pNode);




GBL_DECLS_END

#endif // GIMBAL_LIST_H
