#include <gimbal/strings/gimbal_string_list.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/strings/gimbal_pattern.h>

#define GBL_STRING_LIST_(node) GBL_DOUBLY_LINKED_LIST_ENTRY(node, GblStringList, listNode)

// shared from GblRingList:
void*      GblRingList_new_          (void);
void       GblRingList_delete_       (void* pPtr);
GBL_RESULT GblRingList_insertVaList_ (GblRingList* pSelf, intptr_t index, va_list* pList);


GBL_EXPORT GblStringList* GblStringList_createEmpty(void) {
    return GblRingList_createEmpty();
}

GBL_EXPORT size_t  GblStringList_size(const GblStringList* pSelf) {
    return GblRingList_size(pSelf);
}

GBL_EXPORT GblBool GblStringList_empty(const GblStringList* pSelf) {
    return GblRingList_empty(pSelf);
}

GBL_EXPORT GblStringRef* GblStringList_front(const GblStringList* pSelf) {
    return (GblStringRef*)GblRingList_front(pSelf);
}

GBL_EXPORT GblStringRef* GblStringList_back(const GblStringList* pSelf) {
    return (GblStringRef*)GblRingList_back(pSelf);
}

GBL_EXPORT GblStringRef* GblStringList_at(const GblStringList* pSelf, intptr_t index) {
    return (GblStringRef*)GblRingList_at(pSelf, index);
}

GBL_EXPORT GblBool (GblStringList_contains)(const GblStringList* pSelf, const char* pStr, GblBool matchCase) {
    return GblStringList_find(pSelf, pStr, matchCase) != GBL_STRING_LIST_NPOS? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblBool (GblStringList_splice)(GblStringList* pSelf, GblStringList* pOther, intptr_t index) {
    return GblRingList_splice(pSelf, pOther, index);
}

GBL_EXPORT GblStringRef* GblStringList_popBack(GblStringList* pSelf) {
    return (GblStringRef*)GblRingList_popBack(pSelf);
}

GBL_EXPORT GblStringRef* GblStringList_popFront(GblStringList* pSelf) {
    return (GblStringRef*)GblRingList_popFront(pSelf);
}

GBL_EXPORT void GblStringList_rotate(GblStringList* pSelf, intptr_t n) {
    GblRingList_rotate(pSelf, n);
}

GBL_EXPORT void GblStringList_reverse(GblStringList* pSelf) {
    GblRingList_reverse(pSelf);
}

GBL_EXPORT GblBool (GblStringList_foreach)(GblStringList* pSelf, GblStringListIterFn pFnIt, void* pCl) {
    return GblRingList_foreach(pSelf, (GblRingListIterFn)pFnIt, pCl);
}

GBL_EXPORT GblStringRef* GblStringList_extract(GblStringList* pSelf, GblStringList* pNode) {
    return (GblStringRef*)GblRingList_extract(pSelf, pNode);
}


GBL_EXPORT GblStringList* (GblStringList_create)(const char* pFirst, ...) {
    va_list varArgs;
    GblStringList* pList = GblStringList_createEmpty();
    va_start(varArgs, pFirst);

    GblRingList_pushBack(pList, GblStringRef_create(pFirst));

    const char* pStr;
    while((pStr = va_arg(varArgs, const char*))) {
        GblRingList_pushBack(pList,
                             GblStringRef_create(pStr));

    }

    va_end(varArgs);
    return pList;
}

GBL_EXPORT GblStringList* (GblStringList_createWithRefs)(GblStringRef* pFirst, ...) {
    va_list varArgs;
    GblStringList* pList = GblStringList_createEmpty();
    va_start(varArgs, pFirst);

    GblRingList_pushBack(pList, GblStringRef_acquire(pFirst));

    GblStringRef* pRef;
    while((pRef = va_arg(varArgs, GblStringRef*))) {
        GblRingList_pushBack(pList,
                             GblStringRef_acquire(pRef));

    }

    va_end(varArgs);
    return pList;
}

GBL_EXPORT GblStringList* GblStringList_createSplit(const char* pStr, const char* pDelim) {
    GblStringList* pList = GblStringList_createEmpty();
    GblStringView srcView = GblStringView_fromString(pStr);
    GblStringView delimView = GblStringView_fromString(pDelim);
    size_t  stop = 0;
    size_t  start = 0;

    while(start < srcView.length) {
        stop = GblStringView_findFirstOf(srcView, delimView, start);
        if(stop == GBL_STRING_LIST_NPOS) {
            stop = srcView.length;
        }

        const size_t  length = stop-start;

        if(length) {
            GblStringView subView = GblStringView_fromStringSized(srcView.pData + start,
                                                                  length);
            GblStringRef* pRef = GblStringRef_createFromView(subView);
            GblStringList_pushBackRefs(pList, pRef);
            GblStringRef_release(pRef);
        }

        start = stop + 1;
    }

    return pList;

}

GBL_EXPORT GblStringList* (GblStringList_createFromArray)(const char** ppStrs, size_t  size) {
    size_t  pos = 0;

    GblStringList* pList = GblStringList_createEmpty();

    const char* pStr;
    while(pos < size && (pStr = ppStrs[pos])) {
        GblRingList_pushBack(pList, GblStringRef_create(pStr));
        ++pos;
    }

    return pList;
}

GBL_EXPORT GblStringList* GblStringList_createSubList(const GblStringList* pOther,
                                                      intptr_t             index,
                                                      size_t               count)
{

    GblStringList* pList = NULL;
    GBL_CTX_BEGIN(NULL);

    if(index >= 0)
        GBL_CTX_VERIFY(index + count <= pOther->size,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);
    else
        GBL_CTX_VERIFY(labs(index) + count <= pOther->size+1,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);

    pList = GblStringList_createEmpty();

    GblStringList* pNode = GBL_STRING_LIST_(GblDoublyLinkedList_at(&pOther->listNode, index));
    while(pNode && count) {
        GblRingList_pushBack(pList,
                             GblStringRef_acquire(pNode->pData));
        pNode = (index >= 0)? pNode->ringNode.pNext : pNode->ringNode.pPrev;
        --count;
    }

    GBL_CTX_END_BLOCK();
    return pList;
}

GBL_EXPORT GblStringList* GblStringList_copy(const GblStringList* pOther) {
    GblStringList* pList = GblStringList_createEmpty();

    for(GblStringList* pNode = pOther->ringNode.pNext;
        pNode != pOther;
        pNode = pNode->ringNode.pNext)
    {
        GblRingList_pushBack(pList, GblStringRef_acquire(pNode->pData));
    }

    return pList;
}

GBL_EXPORT GBL_RESULT GblStringList_destroy(GblStringList* pSelf) {
    if(pSelf) {
        for(GblStringList* pNode = pSelf->ringNode.pNext;
            pNode != pSelf;
            pNode = pNode->ringNode.pNext)
        {
            GblStringRef_release(pNode->pData);
        }
        return GblRingList_destroy(pSelf);
    } else return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblBool (GblStringList_equals)(const GblStringList* pSelf, const GblStringList* pOther, GblBool matchCase) {
    if(GblRingList_size(pSelf) != GblRingList_size(pOther)) return GBL_FALSE;

    GblRingList* pIt1 = pSelf->ringNode.pNext;
    GblRingList* pIt2 = pOther->ringNode.pNext;

    while(pIt1 != pSelf && pIt2 != pOther) {

        if(matchCase) {
            if(strcmp(pIt1->pData, pIt2->pData) != 0)
                return GBL_FALSE;
        } else {
            if(!GblStringView_equalsIgnoreCase(GBL_STRV(pIt1->pData),
                                               GBL_STRV(pIt2->pData)))
                return GBL_FALSE;
        }

        pIt1 = pIt1->ringNode.pNext;
        pIt2 = pIt2->ringNode.pNext;
    }

    return GBL_TRUE;
}

GBL_EXPORT size_t  (GblStringList_rfind)(const GblStringList* pSelf, const char* pStr, GblBool matchCase, size_t  offset) {
    size_t  pos = GBL_STRING_LIST_NPOS;
    size_t  i = GblStringList_size(pSelf) - 1;

    if(offset == GBL_STRING_LIST_NPOS || offset == GblStringList_size(pSelf))
        offset = GblStringList_size(pSelf) - 1;

    GblStringView strView = GblStringView_fromString(pStr);
    for(GblStringList* pNode = pSelf->ringNode.pPrev;
        pNode != pSelf;
        pNode = pNode->ringNode.pPrev)
    {
        if(i <= offset) {
            if(matchCase && strcmp(pNode->pData, pStr) == 0) {
                pos = i;
                break;
            } else if(!matchCase &&
                      GblStringView_equalsIgnoreCase(GBL_STRV(pNode->pData),
                                                     strView))
            {
                pos = i;
                break;
            }
        }

        --i;
    }

    return pos;
}

GBL_EXPORT size_t  (GblStringList_find)(const GblStringList* pSelf, const char* pStr, GblBool matchCase, size_t  offset) {
    size_t  pos = GBL_STRING_LIST_NPOS;
    size_t  i = 0;

    GblStringView strView = GblStringView_fromString(pStr);
    for(GblStringList* pNode = pSelf->ringNode.pNext;
        pNode != pSelf;
        pNode = pNode->ringNode.pNext)
    {
        if(i >= offset) {
            if(matchCase && strcmp(pNode->pData, pStr) == 0) {
                pos = i;
                break;
            } else if(!matchCase &&
                      GblStringView_equalsIgnoreCase(GBL_STRV(pNode->pData),
                                                     strView))
            {
                pos = i;
                break;
            }
        }

        ++i;
    }

    return pos;
}

GBL_EXPORT size_t  (GblStringList_count)(const GblStringList* pSelf, const char* pStr, GblBool matchCase) {
    size_t  index = 0;
    size_t  count = 0;

    while((index = GblStringList_find(pSelf, pStr, matchCase, index)) != GBL_STRING_LIST_NPOS) {
        ++count;
        if(++index >= GblStringList_size(pSelf)) break;
    }

    return count;
}

GBL_EXPORT GBL_RESULT GblStringList_clear(GblStringList* pSelf) {
    for(GblStringList* pNode = pSelf->ringNode.pNext;
        pNode != pSelf;
        pNode = pNode->ringNode.pNext)
    {
        GblStringRef_release(pNode->pData);
    }
    return GblRingList_clear(pSelf);
}


GBL_EXPORT GblStringRef* GblStringList_join(const GblStringList* pSelf, const char* pSeparator) {
    struct {
        GblStringBuffer str;
        char extra[1024];
    } buff;

    GblStringRef* pRef = NULL;
    GblStringBuffer_construct(&buff.str, GBL_STRV(""), sizeof(buff));

    for(GblStringList* pNode = pSelf->ringNode.pNext;
        pNode != pSelf;
        pNode = pNode->ringNode.pNext)
    {
        GblStringBuffer_append(&buff.str, GBL_STRV(pNode->pData));
        if(pNode->ringNode.pNext != pSelf)
            GblStringBuffer_append(&buff.str, GBL_STRV(pSeparator));
    }

    pRef = GblStringRef_create(GblStringBuffer_cString(&buff.str));
    GblStringBuffer_destruct(&buff.str);

    return pRef;
}

static GBL_RESULT GblStringList_pushBackVaList_(GblStringList* pSelf, va_list * pList) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    const char* pStr;
    while((pStr = va_arg(*pList, const char*))) {
        result = GblRingList_pushBack(pSelf, GblStringRef_create(pStr));
        if(!GBL_RESULT_SUCCESS(result))
            break;
    }

    return result;

}

GBL_EXPORT GBL_RESULT (GblStringList_pushBack)(GblStringList* pSelf, ...) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    va_list varArgs;
    va_start(varArgs, pSelf);
    result = GblStringList_pushBackVaList_(pSelf, &varArgs);
    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT (GblStringList_pushBackRefs)(GblStringList* pSelf, ...) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    va_list varArgs;
    va_start(varArgs, pSelf);

    GblStringRef* pStr;
    while((pStr = va_arg(varArgs, GblStringRef*))) {
        result = GblRingList_pushBack(pSelf, GblStringRef_acquire(pStr));
        if(!GBL_RESULT_SUCCESS(result))
            break;
    }

    va_end(varArgs);
    return result;
}

static GBL_RESULT GblStringList_pushFrontVaList_(GblStringList* pSelf, va_list* pList) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    size_t  index = 0;
    const char* pStr;
    while((pStr = va_arg(*pList, const char*))) {
        result = GblRingList_insert(pSelf, index++, GblStringRef_create(pStr));
        if(!GBL_RESULT_SUCCESS(result))
            break;
    }
    return result;
}

GBL_EXPORT GBL_RESULT (GblStringList_pushFront)(GblStringList* pSelf, ...) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    va_list varArgs;
    va_start(varArgs, pSelf);
    result = GblStringList_pushFrontVaList_(pSelf, &varArgs);
    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT (GblStringList_pushFrontRefs)(GblStringList* pSelf, ...) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    va_list varArgs;
    va_start(varArgs, pSelf);

    size_t  index = 0;
    GblStringRef* pStr;
    while((pStr = va_arg(varArgs, GblStringRef*))) {
        result = GblRingList_insert(pSelf, index++, GblStringRef_acquire(pStr));
        if(!GBL_RESULT_SUCCESS(result))
            break;
    }

    va_end(varArgs);
    return result;
}

// Should this only put the matched portion in the filtered list?
GBL_EXPORT GblStringList* GblStringList_createFilter(const GblStringList* pOther,
                                                     const char*          pPattern) {
    GblStringList* pList = GblStringList_createEmpty();

    const GblPattern* pCompiled = GblPattern_create(pPattern);

    for(GblStringList* pNode = pOther->ringNode.pNext;
        pNode != pOther;
        pNode = pNode->ringNode.pNext)
    {
        if(GblPattern_match(pCompiled, pNode->pData)) {
            GblRingList_pushBack(pList, GblStringRef_ref(pNode->pData));
        }
    }

    GblPattern_unref(pCompiled);

    return pList;
}


GBL_EXPORT GBL_RESULT (GblStringList_insert)(GblStringList* pSelf, intptr_t index, ...) {
    va_list varArgs;
    va_start(varArgs, index);
    GBL_CTX_BEGIN(NULL);
    if(index == 0) {
        GBL_CTX_VERIFY_CALL(GblStringList_pushFrontVaList_(pSelf, &varArgs));
    } else if(index == (intmax_t)pSelf->size) {
        GBL_CTX_VERIFY_CALL(GblStringList_pushBackVaList_(pSelf, &varArgs));
    } else if(index == -((intmax_t)pSelf->size + 1)) {
        GBL_CTX_VERIFY_CALL(GblStringList_pushFrontVaList_(pSelf, &varArgs));
    } else {
        const char* pArg;
        GblRingList* pEntry = GBL_DOUBLY_LINKED_LIST_ENTRY(GblDoublyLinkedList_at(&pSelf->listNode,
                                                                                  index),
                                                           GblRingList, listNode);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_VERIFY(pEntry, GBL_RESULT_ERROR_OUT_OF_RANGE);

        if(index > 0) {
            while((pArg = va_arg(varArgs, const char*))) {
                GblRingList* pNewEntry = GblRingList_new_();
                GblDoublyLinkedList_init(&pNewEntry->listNode);
                GblDoublyLinkedList_insertBefore(&pEntry->listNode, &pNewEntry->listNode);
                pNewEntry->pData = GblStringRef_create(pArg);;
                ++pSelf->size;
            }

        } else {
            while((pArg = va_arg(varArgs, const char*))) {
                GblRingList* pNewEntry = GblRingList_new_();
                GblDoublyLinkedList_init(&pNewEntry->listNode);
                GblDoublyLinkedList_insertAfter(&pEntry->listNode, &pNewEntry->listNode);
                pNewEntry->pData = GblStringRef_create(pArg);
                ++pSelf->size;
                pEntry = pNewEntry;
            }
        }
    }
    GBL_CTX_END();
    va_end(varArgs);
}

GBL_EXPORT GBL_RESULT (GblStringList_insertRefs)(GblStringList* pSelf, intptr_t index, ...) {
    va_list varArgs;
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    va_start(varArgs, index);

    // store a copy of va args for later
    va_list varArgs2;
    va_copy(varArgs2, varArgs);

    // insert pointers into RingList regularly
    result = GblRingList_insertVaList(pSelf, index, &varArgs);

    // ONLY if insertion was successful, add reference to va arg strings
    if(GBL_RESULT_SUCCESS(result)) {
        GblStringRef* pRef;
        while((pRef = va_arg(varArgs2, GblStringRef*))) {
            GblStringRef_acquire(pRef);
        }
    }

    va_end(varArgs2);
    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT (GblStringList_erase)(GblStringList* pSelf, intptr_t index, size_t  count) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);

    if(index >= 0)
        GBL_CTX_VERIFY(index + count <= pSelf->size,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);
    else
        GBL_CTX_VERIFY(labs(index) + count <= pSelf->size+1,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);


    GblStringList* pNode = GBL_STRING_LIST_(GblDoublyLinkedList_at(&pSelf->listNode, index));
    GblStringList* pNext = NULL;
    while(pNode && count && pSelf->size) {
        pNext = (index >= 0)? pNode->ringNode.pNext : pNode->ringNode.pPrev;
        GblDoublyLinkedList_remove(&pNode->listNode);
        GblStringRef_release(pNode->pData);
        GblRingList_delete_(pNode);
        --count;
        --pSelf->size;
        pNode = pNext;
    }

    GBL_CTX_END();
}


static int GblStringList_sortComparator_(const void* pVal1, const void* pVal2, void* pClosure) {
    if(*((GblBool*)pClosure))
        return strcmp(pVal1, pVal2);
    else
        return strcmp(pVal2, pVal2);
}

GBL_EXPORT void (GblStringList_sort)(GblStringList* pSelf, GblBool ascending) {
    GblRingList_sort(pSelf, GblStringList_sortComparator_, &ascending);
}

GBL_EXPORT size_t  (GblStringList_remove)(GblStringList* pSelf, const char* pStr, GblBool matchCase) {
    size_t  count = 0;
    for(GblStringList* pNode = pSelf->ringNode.pNext;
        pNode != pSelf;
        pNode = pNode->ringNode.pNext)
    {
        if((matchCase && strcmp(pNode->pData, pStr) == 0) ||
           (!matchCase && GblStringView_equalsIgnoreCase(GBL_STRV(pNode->pData),
                                                         GBL_STRV(pStr))))
        {
            GblStringList* pPrev = pNode->ringNode.pPrev;
            GblDoublyLinkedList_remove(&pNode->listNode);
            GblStringRef_release(pNode->pData);
            GblRingList_delete_(pNode);
            pNode = pPrev;
            --pSelf->size;
            ++count;
        }
    }
    return count;
}

GBL_EXPORT GBL_RESULT (GblStringList_deduplicate)(GblStringList* pSelf, GblBool matchCase) {
    for(GblStringList* pNode = pSelf->ringNode.pNext;
        pNode != pSelf;
        pNode = pNode->ringNode.pNext)
    {
        for(GblStringList* pNode2 = pNode->ringNode.pNext;
            pNode2 != pSelf;
            pNode2 = pNode2->ringNode.pNext)
        {

            if((matchCase && strcmp(pNode->pData, pNode2->pData) == 0) ||
               (!matchCase && GblStringView_equalsIgnoreCase(GBL_STRV(pNode->pData),
                                                             GBL_STRV(pNode2->pData))))
            {
                GblStringList* pPrev = pNode2->ringNode.pPrev;
                GblDoublyLinkedList_remove(&pNode2->listNode);
                GblStringRef_release(pNode2->pData);
                GblRingList_delete_(pNode2);
                pNode2 = pPrev;
                --pSelf->size;
            }
        }
    }
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GblStringList_set)(GblStringList* pSelf, intptr_t index, const char* pData) {
    GblStringRef* pRef = GblStringRef_create(pData);
    GblStringRef* pOld = GblRingList_replace(pSelf, index, pRef);
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    if(pOld) {
        GblStringRef_release(pOld);
    } else {
        result = GBL_CTX_LAST_RESULT();
        GblStringRef_release(pRef);
    }

    return result;
}

GBL_EXPORT GBL_RESULT (GblStringList_setRef)(GblStringList* pSelf, intptr_t index, GblStringRef* pRef) {
    GblStringRef* pOld = GblRingList_replace(pSelf, index, GblStringRef_acquire(pRef));
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    if(pOld) {
        GblStringRef_release(pOld);
    } else {
        result = GBL_CTX_LAST_RESULT();
        GblStringRef_release(pRef);
    }
    return result;
}

GBL_EXPORT size_t  (GblStringList_replace)(GblStringList* pSelf,
                                           const char*    pOld,
                                           const char*    pNew,
                                           GblBool        matchCase,
                                           size_t         limit)
{
    size_t  foundCount = 0;
    size_t  index;
    size_t  offset = 0;
    while((index = GblStringList_find(pSelf, pOld, matchCase, offset)) != GBL_STRING_LIST_NPOS) {
        GblStringList_set(pSelf, index, pNew);
        if(++foundCount == limit && limit) break;
        offset = index + 1;
        if(offset >= GblStringList_size(pSelf)) break;
    }
    return foundCount;
}

GBL_EXPORT size_t  (GblStringList_replaceWithRef)(GblStringList* pSelf,
                                                  const char*    pOld,
                                                  GblStringRef*  pNew,
                                                  GblBool        matchCase,
                                                  size_t         limit) {
    size_t  foundCount = 0;
    size_t  index;
    size_t  offset = 0;
    while((index = GblStringList_find(pSelf, pOld, matchCase, offset)) != GBL_STRING_LIST_NPOS) {
        GblStringList_setRef(pSelf, index, pNew);
        if(++foundCount == limit && limit) break;
        offset = index + 1;
        if(offset >= GblStringList_size(pSelf)) break;
    }
    return foundCount;
}
