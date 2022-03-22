#ifndef GIMBAL_LIST_H
#define GIMBAL_LIST_H

#include "../types/gimbal_typedefs.h"

GBL_DECLS_BEGIN

typedef struct GblListNode {
    struct GblListNode* pNext;
} GblListNode;

typedef struct GblDListNode {
    union {
        struct GblDListNode*   pNext;
        GblListNode            base;
    };
   struct GblDListNode*   pPrev;
} GblDListNode;


GBL_API gblListNodeInsertAfter(GblListNode* pNode);
GBL_API gblListNodeRemoveAfter(GblListNode* pNode);
GBL_API gblListNodeCountAfter(GblListNode* pNode);

GBL_API gblDListNodeInsertBefore(GblDListNode* pNode);
GBL_API gblDListNodeRemoveBefore(GblDListNode* pNode);
GBL_API gblDListNodeCount(GblDListNode* pNode);


GBL_DECLS_END

#endif // GIMBAL_LIST_H
