/*! \file
 *  \brief GblNaryTreeNode structure and related functions
 *  \todo Finish implementing commented out API operations
 *  \ingroup containers
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_NARY_TREE_H
#define GIMBAL_NARY_TREE_H

#include "../core/gimbal_ctx.h"

#define GBL_NARY_TREE_NPOS                           GBL_NPOS
#define GBL_NARY_TREE_ENTRY(node, structure, field)  GBL_CONTAINER_OF(node, structure, field)
#define GBL_NARY_TREE_TRAVERSAL_MASK(order, flags)   ((order << 0x3) | (flags))

#define GBL_SELF_TYPE GblNaryTreeNode

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblNaryTreeNode);

typedef GblBool (*GblNaryTreeIterFn)(const GblNaryTreeNode* pNode, void* pUd);

typedef enum GBL_NARY_TREE_NODE_FLAGS {
    GBL_NARY_TREE_NODE_FLAG_ROOT     = 0x1,
    GBL_NARY_TREE_NODE_FLAG_INTERNAL = 0x2,
    GBL_NARY_TREE_NODE_FLAG_LEAF     = 0x4,
    GBL_NARY_TREE_NODE_FLAGS_ALL     = 0x7
} GBL_NARY_TREE_NODE_FLAGS;

typedef enum GBL_NARY_TREE_TRAVERSAL_ORDER {
    GBL_NARY_TREE_TRAVERSAL_ORDER_PRE,
    GBL_NARY_TREE_TRAVERSAL_ORDER_IN,
    GBL_NARY_TREE_TRAVERSAL_ORDER_POST,
    GBL_NARY_TREE_TRAVERSAL_ORDER_LEVEL
} GBL_NARY_TREE_TRAVERSAL_ORDER;

/*! \brief Represents a single intrusive node within an N-Ary tree structure
 *
 *  GblNaryTreeNode is a single node which is to be embedded within another
 *  structure, providing heirarchial relationships between its instances. As
 *  relationships are established between these instances, they become nodes
 *  comprising an aggregate tree structure.
 *
 *  A single node contains only a pointer to its parent node plus a linked list of
 *  its child nodes, resulting in a minimal memory footprint. The API attempts to
 *  present psuedo list-style semantics for accessing ancestors, bases, siblings,
 *  and children by index; however, these are linked structures and such operations
 *  are of O(N) time complexity, requiring traversals for random access.
 *
 *  \ingroup containers
 */
typedef struct GblNaryTreeNode {
    struct GblNaryTreeNode* pParent;        ///< Node's parent
    struct GblNaryTreeNode* pChildFirst;    ///< Node's first child (beginning of child linked list)
    struct GblNaryTreeNode* pSiblingNext;   ///< Node's next sibling (next entry of child linked list)
} GblNaryTreeNode;

//GBL_EXPORT size_t         GblNaryTree_size                (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_EXPORT size_t         GblNaryTree_width               (GBL_CSELF, size_t  depth);                                    GBL_NOEXCEPT;
//GBL_EXPORT size_t         GblNaryTree_height              (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT size_t           GblNaryTree_depth               (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_EXPORT size_t         GblNaryTree_breadth             (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_EXPORT size_t         GblNaryTree_degree              (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_EXPORT size_t         GblNaryTree_arity               (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblFlags         GblNaryTree_flags               (GBL_CSELF)                                                    GBL_NOEXCEPT;

GBL_EXPORT GblBool          GblNaryTree_isConnected         (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_isRoot              (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_isInternal          (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_isLeaf              (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_EXPORT GblBool        GblNaryTree_isBalanced          (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_isParent            (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_isAncestor          (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_isSibling           (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_isChild             (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_isDescendent        (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
//GBL_EXPORT GblBool        GblNaryTree_isRelative          (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;

GBL_EXPORT size_t           GblNaryTree_childCount          (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_childLast           (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_childBefore         (GBL_CSELF, const GblNaryTreeNode* pChild)                     GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_childAt             (GBL_CSELF, size_t  index)                                     GBL_NOEXCEPT;
GBL_EXPORT size_t           GblNaryTree_childIndex          (GBL_CSELF, const GblNaryTreeNode* pChild)                     GBL_NOEXCEPT;

GBL_EXPORT void             GblNaryTree_addChildFront       (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_EXPORT void             GblNaryTree_addChildBack        (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_EXPORT void             GblNaryTree_addChildTo          (GBL_SELF, size_t  index, GblNaryTreeNode* pChild)             GBL_NOEXCEPT;
GBL_EXPORT void             GblNaryTree_addChildBefore      (GBL_SELF, GblNaryTreeNode* pBefore,GblNaryTreeNode* pChild)   GBL_NOEXCEPT;
GBL_EXPORT void             GblNaryTree_addChildAfter       (GBL_SELF, GblNaryTreeNode* pAfter, GblNaryTreeNode* pChild)   GBL_NOEXCEPT;

GBL_EXPORT void             GblNaryTree_moveChildFront      (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_EXPORT void             GblNaryTree_moveChildBack       (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_EXPORT void             GblNaryTree_moveChildTo         (GBL_SELF, size_t  index, GblNaryTreeNode* pChild)             GBL_NOEXCEPT;

GBL_EXPORT GblNaryTreeNode* GblNaryTree_removeChild         (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_removeChildFront    (GBL_SELF)                                                     GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_removeChildBack     (GBL_SELF)                                                     GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_removeChildAt       (GBL_SELF, size_t  index)                                      GBL_NOEXCEPT;

GBL_EXPORT void             GblNaryTree_replaceChild        (GBL_SELF, GblNaryTreeNode* pOld, GblNaryTreeNode* pNew)       GBL_NOEXCEPT;
GBL_EXPORT void             GblNaryTree_replaceChildAt      (GBL_SELF, size_t  index, GblNaryTreeNode* pNewChild)          GBL_NOEXCEPT;

GBL_EXPORT void             GblNaryTree_swapChildren        (GBL_SELF, GblNaryTreeNode* pChild1, GblNaryTreeNode* pChild2) GBL_NOEXCEPT;
GBL_EXPORT void             GblNaryTree_swapChildrenAt      (GBL_SELF, size_t  index1, size_t  index2)                     GBL_NOEXCEPT;

GBL_EXPORT void             GblNaryTree_reverseChildren     (GBL_SELF)                                                     GBL_NOEXCEPT;

GBL_EXPORT GblNaryTreeNode* GblNaryTree_root                (GBL_SELF)                                                     GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_base                (GBL_SELF, size_t  depth)                                      GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_ancestor            (GBL_CSELF, size_t  height)                                    GBL_NOEXCEPT;
GBL_EXPORT size_t           GblNaryTree_ancestorHeight      (GBL_CSELF, const GblNaryTreeNode* pParent)                    GBL_NOEXCEPT;

GBL_EXPORT size_t           GblNaryTree_siblingCount        (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_siblingLast         (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_siblingBefore       (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_siblingFirst        (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblNaryTreeNode* GblNaryTree_siblingAt           (GBL_CSELF, size_t  index)                                     GBL_NOEXCEPT;
GBL_EXPORT size_t           GblNaryTree_siblingIndex        (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;

GBL_EXPORT void             GblNaryTree_disconnect          (GBL_SELF)                                                     GBL_NOEXCEPT;

GBL_EXPORT GblNaryTreeNode* GblNaryTree_lowestCommonAncestor(GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
//GBL_EXPORT size_t           GblNaryTree_distance            (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_traverse            (GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIt, void* pUd) GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_traverseInOrder     (GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_traversePreOrder    (GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblNaryTree_traversePostOrder   (GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_NARY_TREE_H
