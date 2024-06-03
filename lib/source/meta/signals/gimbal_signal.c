#include <gimbal/meta/signals/gimbal_signal.h>
#include <gimbal/meta/signals/gimbal_c_closure.h>
#include <gimbal/meta/signals/gimbal_class_closure.h>
#include <gimbal/meta/signals/gimbal_signal_closure.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/containers/gimbal_array_map.h>
#include <gimbal/containers/gimbal_array_list.h>
#include <gimbal/containers/gimbal_doubly_linked_list.h>
#include <gimbal/allocators/gimbal_pool_allocator.h>
#include "../types/gimbal_type_.h"

#define GBL_CONNECTION_POOL_ALLOCATOR_

#ifdef GBL_CONNECTION_POOL_ALLOCATOR_
#   define GBL_CONNECTION_POOL_ALLOCATOR_PAGE_SIZE_  16
#   define GBL_CONNECTION_NEW_()                     (GblPoolAllocator_new(&connectionAllocator_))
#   define GBL_CONNECTION_DELETE_(ptr)               (GblPoolAllocator_delete(&connectionAllocator_, ptr))
#else
#   define GBL_CONNECTION_NEW_()                     GBL_CTX_NEW(Connection_)
#   define GBL_CONNECTION_DELETE_(ptr)               GBL_CTX_FREE(ptr)
#endif

typedef struct Signal_ {
    GblType         ownerType;
    GblQuark        name;
    //GblLinkedListnNode next;
    GblMarshalFn    pFnCMarshal;
    size_t          argCount;
    GblType         argTypes[];
} Signal_;

typedef struct Connection_ {
    GblDoublyLinkedListNode emitterList;
    GblDoublyLinkedListNode receiverList;
    GblInstance*            pEmitter;
    GblInstance*            pReceiver;
    const Signal_*          pSignal;
    GblClosure*             pClosure;
} Connection_;

typedef struct EmitterHandler_ {
    GblDoublyLinkedListNode connectionList;
    GblBool                 blocked;
} EmitterHandler_;

typedef struct InstanceConnectionTable_ {
    GblInstance*                pInstance;
    GblDoublyLinkedListNode     receiverConnections;
    GblArrayMap*                pEmitterHandlers;
    GblBool                     signalsBlocked;
} InstanceConnectionTable_;

static GblHashSet       signalSet_;
static GblHashSet       instanceConnectionTableSet_;
#ifdef GBL_CONNECTION_POOL_ALLOCATOR_
static GblPoolAllocator connectionAllocator_;
#endif

static GBL_THREAD_LOCAL Connection_* pActiveConnection_ = NULL;

static GblHash signalSetHasher_(const GblHashSet* pSet, const void* pEntry) {
    GBL_UNUSED(pSet);
    const Signal_* pSignal = *(const Signal_**)pEntry;
    return gblHash(pSignal, sizeof(GblType) + sizeof(GblQuark));
}

static GblBool signalSetComparator_(const GblHashSet* pSet, const void* pEntry1, const void* pEntry2) {
    GBL_UNUSED(pSet);
    const Signal_* pSignal1 = *(const Signal_**)pEntry1;
    const Signal_* pSignal2 = *(const Signal_**)pEntry2;
    return pSignal1->ownerType == pSignal2->ownerType && pSignal1->name == pSignal2->name;
}

static void signalSetDestructor_(const GblHashSet* pSet, void* pEntry) {
    GBL_UNUSED(pSet);
    GBL_CTX_BEGIN(GblHashSet_context(pSet));
    GBL_CTX_FREE(*(Signal_**)pEntry);
    GBL_CTX_END_BLOCK();
}

GBL_EXPORT GBL_RESULT GblSignal_install(GblType      ownerType,
                                        const char*  pName,
                                        GblMarshalFn pFnCMarshal,
                                        size_t       argCount,
                                        ...)
{
    va_list varArgs;
    va_start(varArgs, argCount);
    Signal_* pSignal = NULL;

    GBL_CTX_BEGIN(GblHashSet_context(&signalSet_));
    GBL_CTX_PUSH_VERBOSE("[GblSignal] Installing signal: [%s::%s]",
                         GblType_name(ownerType),
                         pName);

    GBL_CTX_VERIFY_TYPE(ownerType);
    GBL_CTX_VERIFY_POINTER(pName);

    const size_t  allocSize = sizeof(Signal_) + sizeof(GblType)*argCount;

    pSignal = GBL_CTX_MALLOC(gblAlignedAllocSizeDefault(allocSize),
                             GBL_ALIGNOF(GBL_MAX_ALIGN_T),
                             pName);

    pSignal->ownerType      = ownerType;
    pSignal->name           = GblQuark_fromString(pName);
    pSignal->pFnCMarshal    = pFnCMarshal;
    pSignal->argCount       = argCount;

    for(size_t  a = 0; a < argCount; ++a) {
        pSignal->argTypes[a] = va_arg(varArgs, GblType);
        GBL_CTX_VERIFY_TYPE(pSignal->argTypes[a]);
    }

    void* pExisting = GblHashSet_set(&signalSet_, &pSignal);

    if(pExisting) {
        GBL_CTX_WARN("Overwrote existing signal! [%s::%s]",
                     GblType_name(ownerType),
                     pName);
    }


    GBL_CTX_POP(1);
    GBL_CTX_END_BLOCK();

    if(!GBL_RESULT_SUCCESS(GBL_CTX_RESULT())) {
        GBL_CTX_FREE(pSignal);
    }

    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblSignal_uninstall(GblType     ownerType,
                                          const char* pName)
{
    GBL_CTX_BEGIN(GblHashSet_context(&signalSet_));
    GBL_CTX_PUSH_VERBOSE("[GblSignal] Uninstalling signal: [%s::%s]",
                         GblType_name(ownerType), pName);

    Signal_* pKey = GBL_ALLOCA(sizeof(Signal_));
    pKey->ownerType = ownerType;
    pKey->name      = GblQuark_fromString(pName);

    GBL_CTX_VERIFY(GblHashSet_erase(&signalSet_, &pKey),
                   GBL_RESULT_ERROR_INVALID_HANDLE);

    GBL_CTX_POP(1);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblSignal_uninstallAll(GblType ownerType) {
    GBL_CTX_BEGIN(GblHashSet_context(&signalSet_));
    GBL_CTX_PUSH_VERBOSE("[GblSignal] Uninstalling all signals: [%s]",
                         GblType_name(ownerType));

    GBL_CTX_WARN("UNIMPLEMENTED!!\n");

    GBL_CTX_POP(1);
    GBL_CTX_END();
}

static Signal_* Signal_findByQuark_(GblType     ownerType,
                                    GblQuark    name)
{
    Signal_* pSignal  = NULL;
    Signal_* pKey     = GBL_ALLOCA(sizeof(Signal_));
    pKey->name          = name;

    GblType curType = ownerType;

    // iterate over bases
    while(GblType_verify(curType)) {
        pKey->ownerType = curType;

        // check current type
        Signal_** ppSignal = GblHashSet_get(&signalSet_, &pKey);
        if(ppSignal) {
            pSignal = *ppSignal;
            break;
        }

        // check interfaces on current type
        const GblMetaClass* pMeta = GBL_META_CLASS_(curType);
        for(uint8_t i = 0; i < pMeta->pInfo->interfaceCount; ++i) {
            pSignal = Signal_findByQuark_(pMeta->pInfo->pInterfaceImpls[i].interfaceType,
                                             name);
            if(pSignal) goto done;
        }

        curType = GblType_parent(curType);
    }
done:
    return pSignal;
}

static GblHash instanceConnectionTableHasher_(const GblHashSet* pSet, const void* pEntry) {
    GBL_UNUSED(pSet);
    const InstanceConnectionTable_* pConnections = *(const InstanceConnectionTable_**)pEntry;
    return gblHash(&pConnections->pInstance, sizeof(GblInstance*));
}

static GblBool instanceConnectionTableComparator_(const GblHashSet* pSet, const void* pEntry1, const void* pEntry2) {
    GBL_UNUSED(pSet);
    const InstanceConnectionTable_* pConnections1 = *(const InstanceConnectionTable_**)pEntry1;
    const InstanceConnectionTable_* pConnections2 = *(const InstanceConnectionTable_**)pEntry2;
    return (pConnections1->pInstance == pConnections2->pInstance);
}

static void instanceConnectionTableDestructor_(const GblHashSet* pSet, void* pEntry) {
    GBL_CTX_BEGIN(GblHashSet_context(pSet));
    InstanceConnectionTable_* pConnections = *(InstanceConnectionTable_**)pEntry;

#ifndef NDEBUG
    GBL_CTX_VERIFY_EXPRESSION(!GblDoublyLinkedList_count(&pConnections->receiverConnections));
    for(size_t  s = 0; s < GblArrayMap_size(&pConnections->pEmitterHandlers); ++s) {
       EmitterHandler_* pHandler = (void*)GblArrayMap_probeValue(&pConnections->pEmitterHandlers,
                                                                s);
       GBL_CTX_VERIFY_EXPRESSION(!GblDoublyLinkedList_count(&pHandler->connectionList));

    }
#endif
    GblArrayMap_destroy(&pConnections->pEmitterHandlers);
    GBL_CTX_FREE(pConnections);
    GBL_CTX_END_BLOCK();
}

static InstanceConnectionTable_* InstanceConnectionTable_find_(GblInstance* pInstance) {
    InstanceConnectionTable_* pKey = GBL_ALLOCA(sizeof(InstanceConnectionTable_));
    pKey->pInstance = pInstance;
    InstanceConnectionTable_** ppTable = GblHashSet_get(&instanceConnectionTableSet_,
                                                       &pKey);
    return ppTable? *ppTable : NULL;
}

static InstanceConnectionTable_* InstanceConnectionTable_create_(GblInstance* pInstance) {
    InstanceConnectionTable_* pTable = NULL;
    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));
    GBL_CTX_VERIFY_POINTER(pInstance);
    pTable = GBL_CTX_MALLOC(sizeof(InstanceConnectionTable_));

    memset(pTable, 0, sizeof(InstanceConnectionTable_));
    pTable->pInstance = pInstance;

    GblDoublyLinkedList_init(&pTable->receiverConnections);

    GBL_CTX_VERIFY(GblHashSet_insert(&instanceConnectionTableSet_, &pTable),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Failed to create an emitter connection table for type [%s]!",
                   GblType_name(GBL_TYPEOF(pInstance)));
    GBL_CTX_END_BLOCK();
    return pTable;
}

GBL_INLINE InstanceConnectionTable_* InstanceConnectionTable_findOrCreate_(GblInstance* pInstance) {
    InstanceConnectionTable_* pTable = InstanceConnectionTable_find_(pInstance);
    if(!pTable) {
        pTable = InstanceConnectionTable_create_(pInstance);
    }
    return pTable;
}


static GBL_RESULT emitterSignalHandlerDtor_(const GblArrayMap* pMap, uintptr_t key, void* pEntry) {
    GBL_UNUSED(key, pMap);
    EmitterHandler_* pHandlers = pEntry;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_FREE(pHandlers);
    GBL_CTX_END();
}

static EmitterHandler_* EmitterHandler_find_(GblInstance*               pInstance,
                                             const char*                pName,
                                             GblQuark                   nameQuark,
                                             InstanceConnectionTable_*  pTable)
{
    EmitterHandler_* pHandler = NULL;

    if(!pTable)
        pTable = InstanceConnectionTable_find_(pInstance);

    if(pTable) {

        if(nameQuark == GBL_QUARK_INVALID)
            nameQuark = GblQuark_tryString(pName);

        if(nameQuark != GBL_QUARK_INVALID)
            pHandler = (void*)GblArrayMap_getValue(&pTable->pEmitterHandlers,
                                                  nameQuark);
    }

    return pHandler;
}



static EmitterHandler_* EmitterHandler_findOrCreate_(GblInstance*               pInstance,
                                                     const char*                pName,
                                                     GblQuark                   nameQuark,
                                                     InstanceConnectionTable_*  pTable,
                                                     Signal_*                   pSignal,
                                                     InstanceConnectionTable_** ppTableOut,
                                                     Signal_**                  ppSignalOut)
{
    EmitterHandler_* pHandler = NULL;
    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));

    GBL_CTX_VERIFY_POINTER(pInstance);

    if(nameQuark == GBL_QUARK_INVALID) {
        if(pSignal) nameQuark = pSignal->name;
        else nameQuark = GblQuark_tryString(pName);
    }

    GBL_CTX_VERIFY(nameQuark != GBL_QUARK_INVALID,
                   GBL_RESULT_ERROR_INVALID_HANDLE,
                   "[GblType] %s isn't even a quark, no way it can be a signal!",
                   pName);

    // first check to see if we have an entry for the signal (fast path)
    if(!pTable) {
        pTable = InstanceConnectionTable_find_(pInstance);
        if(ppTableOut) *ppTableOut = pTable;
    }

    pHandler = EmitterHandler_find_(pInstance, pName, nameQuark, pTable);

    if(!pHandler || ppSignalOut) {
        //then check to see if signal is even valid
        const GblType typeId = GBL_TYPEOF(pInstance);
        if(!pSignal) pSignal = Signal_findByQuark_(typeId,
                                                   nameQuark);
        GBL_CTX_VERIFY(pSignal,
                       GBL_RESULT_ERROR_INVALID_HANDLE,
                       "[GblType] %s is not a valid signal on type %s",
                       GblQuark_toString(nameQuark), GblType_name(typeId));

        if(ppSignalOut) *ppSignalOut = pSignal;
    }

    if(!pHandler && pSignal) {

        //then create table if necessary
        if(!pTable) pTable = InstanceConnectionTable_create_(pInstance);

        if(ppTableOut) *ppTableOut = pTable;

        //finally add the emitter handler to the table for the given signal
        pHandler = GBL_CTX_MALLOC(sizeof(EmitterHandler_));
        memset(pHandler, 0, sizeof(EmitterHandler_));
        GblDoublyLinkedList_init(&pHandler->connectionList);

        GblArrayMap_setUserdata(&pTable->pEmitterHandlers,
                                nameQuark,
                                (uintptr_t)pHandler,
                                emitterSignalHandlerDtor_);
    }

    GBL_CTX_END_BLOCK();
    return pHandler;

}

static GBL_RESULT Signal_connect_(GblInstance*   pEmitter,
                                  const char*    pName,
                                  GblQuark       name,
                                  GblInstance*   pReceiver,
                                  GblClosure*    pClosure)
{
    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));
    GBL_CTX_VERIFY_POINTER(pEmitter);
    GBL_CTX_VERIFY_POINTER(pName);
    GBL_CTX_VERIFY_POINTER(pReceiver);
    GBL_CTX_VERIFY_POINTER(pClosure);

    Signal_* pSignal = NULL;
    InstanceConnectionTable_* pEmitterTable = NULL;

    // Fetch or create handler entry
    //   which will also grab the generated emitter table and signal
    EmitterHandler_* pHandler = EmitterHandler_findOrCreate_(pEmitter,
                                                             pName,
                                                             name,
                                                             NULL,
                                                             NULL,
                                                             &pEmitterTable,
                                                             &pSignal);
    GBL_CTX_VERIFY_LAST_RECORD();

    // Fetch or create receiver connection table
    InstanceConnectionTable_* pReceiverTable = (pReceiver != pEmitter)?
                                                InstanceConnectionTable_findOrCreate_(pReceiver) :
                                                pEmitterTable;

    // Initialize closure
    if(!GblClosure_hasMarshal(pClosure)) GblClosure_setMarshal(pClosure, pSignal->pFnCMarshal);

    Connection_* pConnection = GBL_CONNECTION_NEW_();
    memset(pConnection, 0, sizeof(Connection_));
    pConnection->pEmitter   = pEmitter;
    pConnection->pReceiver  = pReceiver;
    pConnection->pSignal    = pSignal;
    pConnection->pClosure   = pClosure;

    // Add connection to receiver list
    GblDoublyLinkedList_pushBack(&pReceiverTable->receiverConnections,
                                 &pConnection->receiverList);

    // Add connection to emitter list
    GblDoublyLinkedList_pushBack(&pHandler->connectionList,
                                 &pConnection->emitterList);

    GBL_CTX_END_BLOCK();
    if(!GBL_RESULT_SUCCESS(GBL_CTX_RESULT())) {
        GBL_UNREF(pClosure);
    }
    return GBL_CTX_RESULT();
}



GBL_EXPORT GBL_RESULT (GblSignal_connect)(GblInstance* pEmitter,
                                          const char*  pSignalName,
                                          GblInstance* pReceiver,
                                          GblFnPtr     pFnCCallback,
                                          void*        pUserdata)
{
    if(!pFnCCallback)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    else {
        GblCClosure* pCClosure = GblCClosure_create(pFnCCallback, pUserdata);
        return Signal_connect_(pEmitter, pSignalName, GBL_QUARK_INVALID, pReceiver, GBL_CLOSURE(pCClosure));
    }
}

GBL_EXPORT GBL_RESULT GblSignal_connectClass(GblInstance* pEmitter,
                                             const char*  pSignalName,
                                             GblInstance* pReceiver,
                                             GblType      classType,
                                             size_t       methodOffset)
{
    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));

    GBL_CTX_VERIFY(GblInstance_check(pReceiver, classType),
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_CTX_VERIFY_ARG(methodOffset >= sizeof(GblClass));

    GblClassClosure* pCClosure = GblClassClosure_create(classType, methodOffset, pReceiver, NULL);
    GBL_CTX_VERIFY_CALL(Signal_connect_(pEmitter, pSignalName, GBL_QUARK_INVALID, pReceiver, GBL_CLOSURE(pCClosure)));

    GBL_CTX_END();
}


GBL_EXPORT GBL_RESULT GblSignal_connectSignal(GblInstance* pEmitter,
                                              const char*  pSignalName,
                                              GblInstance* pDstEmitter,
                                              const char*  pDstSignalName)
{
    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));
    GblQuark quarkName = GblQuark_tryString(pDstSignalName);

    GBL_CTX_VERIFY(quarkName != GBL_QUARK_INVALID,
                   GBL_RESULT_ERROR_INVALID_HANDLE);

    GBL_CTX_VERIFY(Signal_findByQuark_(GBL_TYPEOF(pDstEmitter), quarkName),
                   GBL_RESULT_ERROR_INVALID_HANDLE);

    GblSignalClosure* pSignalClosure = GblSignalClosure_create(pDstSignalName, NULL);
    GBL_CTX_VERIFY_CALL(Signal_connect_(pEmitter, pSignalName, GBL_QUARK_INVALID, pDstEmitter, GBL_CLOSURE(pSignalClosure)));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblSignal_connectClosure(GblInstance* pInstance,
                                               const char*  pName,
                                               GblInstance* pReceiver,
                                               GblClosure*  pClosure)
{
    return pClosure? Signal_connect_(pInstance, pName, GBL_QUARK_INVALID, pReceiver, pClosure) :
                     GBL_RESULT_ERROR_INVALID_POINTER;
}

GBL_INLINE GBL_RESULT deleteConnection_(Connection_* pConnection) {
    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));

    GblDoublyLinkedList_remove(&pConnection->emitterList);
    GblDoublyLinkedList_remove(&pConnection->receiverList);
    GBL_UNREF(pConnection->pClosure);
    GBL_CONNECTION_DELETE_(pConnection);

    GBL_CTX_END();
}

static size_t  disconnectFromHandler_(EmitterHandler_*  pHandler,
                                      GblClosure*       pClosure)
{
    size_t  disconnectedCount = 0;
    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));
    GblDoublyLinkedListNode* pNext;
    for(GblDoublyLinkedListNode* pNode = pHandler->connectionList.pNext;
        pNode != &pHandler->connectionList;
        pNode = pNext)
    {
        pNext = pNode->pNext;
        Connection_* pConnection = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode, Connection_, emitterList);

        if(!pClosure || pClosure == pConnection->pClosure)
        {
            GBL_CTX_CALL(deleteConnection_(pConnection));
            ++disconnectedCount;
        }
    }
    GBL_CTX_END_BLOCK();
    return disconnectedCount;
}


static size_t  GblSignal_disconnect_(GblInstance*               pEmitter,
                                     const char*                pSignalName,
                                     GblInstance*               pReceiver,
                                     GblClosure*                pClosure,
                                     InstanceConnectionTable_*  pEmitterTable,
                                     InstanceConnectionTable_*  pReceiverTable)
{
    size_t  disconnectedCount = 0;

    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));

    GBL_CTX_VERIFY_POINTER(pEmitter || pReceiver,
                           "[GblSignal] disconnecting requires either an emitter or receiver!");

    const GblQuark signalName = GblQuark_tryString(pSignalName);

    if(pReceiver) {
        if(!pReceiverTable) {
            pReceiverTable = InstanceConnectionTable_find_(pReceiver);
        }

        if(pReceiverTable) {
            GblDoublyLinkedListNode* pNext;
            for(GblDoublyLinkedListNode* pNode = pReceiverTable->receiverConnections.pNext;
                pNode != &pReceiverTable->receiverConnections;
                pNode = pNext)
            {
                pNext = pNode->pNext;
                Connection_* pConnection = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode, Connection_, receiverList);

                if((signalName == GBL_QUARK_INVALID || signalName == pConnection->pSignal->name) &&
                   (!pEmitter                       || pEmitter   == pConnection->pEmitter)      &&
                   (!pClosure                       || pClosure   == pConnection->pClosure))
                {
                    GBL_CTX_CALL(deleteConnection_(pConnection));
                    ++disconnectedCount;
                }
            }
        }
    } if(pEmitter) {

        if(!pEmitterTable) {
            pEmitterTable = InstanceConnectionTable_find_(pEmitter);
        }

        if(pEmitterTable) {

            if(signalName != GBL_QUARK_INVALID) {
                EmitterHandler_* pHandler = (void*)GblArrayMap_getValue(&pEmitterTable->pEmitterHandlers,
                                                                        signalName);
                if(pHandler)
                    disconnectedCount += disconnectFromHandler_(pHandler, pClosure);

            } else {
                const size_t  signalHandlerCount = GblArrayMap_size(&pEmitterTable->pEmitterHandlers);
                for(size_t  s = 0; s < signalHandlerCount; ++s) {
                    EmitterHandler_* pHandler = (void*)GblArrayMap_probeValue(&pEmitterTable->pEmitterHandlers,
                                                                              s);
                    disconnectedCount += disconnectFromHandler_(pHandler, pClosure);
                }
            }
        }
    }

    GBL_CTX_END_BLOCK();
    return disconnectedCount;
}

GBL_EXPORT size_t  GblSignal_disconnect(GblInstance* pEmitter,
                                        const char*  pSignalName,
                                        GblInstance* pReceiver,
                                        GblClosure*  pClosure)
{
    return GblSignal_disconnect_(pEmitter, pSignalName, pReceiver, pClosure, NULL, NULL);
}

// Fix for Windows: "Redefinition with different linkage".
// GBL_EXPORT moved to declaration in gimbal_type_.h
GBL_RESULT GblSignal_removeInstance_(GblInstance* pInstance) {
    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));

    InstanceConnectionTable_* pTable = InstanceConnectionTable_find_(pInstance);
    if(pTable) {

        GblSignal_disconnect_(NULL, NULL, pInstance, NULL, NULL, pTable);
        GblSignal_disconnect_(pInstance, NULL, NULL, NULL, pTable, NULL);

        GBL_CTX_VERIFY(GblHashSet_erase(&instanceConnectionTableSet_,
                                        &pTable),
                       GBL_RESULT_ERROR_MEM_FREE,
                       "[GblSignal] Failed to erase instance connection table!");
    }

    GBL_CTX_END();
}

GBL_EXPORT GblBool GblSignal_block(GblInstance* pInstance,
                                   const char*  pSignalName,
                                   GblBool      blocked)
{
    GblBool old = GBL_FALSE;
    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));

    EmitterHandler_* pHandler = EmitterHandler_findOrCreate_(pInstance,
                                                             pSignalName,
                                                             GBL_QUARK_INVALID,
                                                             NULL,
                                                             NULL,
                                                             NULL,
                                                             NULL);
    GBL_CTX_VERIFY_LAST_RECORD();

    if(pHandler) {
        old = pHandler->blocked;
        pHandler->blocked = blocked;
    }

    GBL_CTX_END_BLOCK();
    return old;
}


GBL_EXPORT GblBool GblSignal_blockAll(GblInstance* pInstance,
                                      GblBool      blocked)
{
    GblBool old = GBL_FALSE;
    InstanceConnectionTable_* pTable = InstanceConnectionTable_findOrCreate_(pInstance);
    if(pTable) {
        old = pTable->signalsBlocked;
        pTable->signalsBlocked = blocked;
    }
    return old;
}

GBL_EXPORT size_t  GblSignal_connectionCount(GblInstance* pInstance,
                                             const char*  pSignalName)
{
    size_t  count = 0;
    if(pSignalName) {
        EmitterHandler_* pHandler = EmitterHandler_find_(pInstance, pSignalName, GBL_QUARK_INVALID, NULL);
        if(pHandler) {
            count = GblDoublyLinkedList_count(&pHandler->connectionList);
        }
    } else {
        InstanceConnectionTable_* pTable = InstanceConnectionTable_find_(pInstance);
        if(pTable) {
            const size_t  size = GblArrayMap_size(&pTable->pEmitterHandlers);
            for(size_t  s = 0; s < size; ++s) {
                EmitterHandler_* pHandler = (EmitterHandler_*)GblArrayMap_probeValue(&pTable->pEmitterHandlers, s);
                count += GblDoublyLinkedList_count(&pHandler->connectionList);
            }
        }
    }
    return count;
}

static GBL_RESULT GblSignal_emit_(GblInstance* pEmitter,
                                  const char*  pSignalName,
                                  va_list*     pVarArgs,
                                  GblVariant*  pVariantArgs)
{

    GBL_CTX_BEGIN(GblHashSet_context(&instanceConnectionTableSet_));
    GBL_CTX_VERIFY_POINTER(pEmitter);
    GBL_CTX_VERIFY_POINTER(pSignalName);

    // if the instance has signal handlers for the specified signal (that aren't blocked)
    InstanceConnectionTable_* pEmitterTable = InstanceConnectionTable_find_(pEmitter);
    if(pEmitterTable && !pEmitterTable->signalsBlocked) {
        EmitterHandler_* pHandler = EmitterHandler_find_(pEmitter, pSignalName, GBL_QUARK_INVALID, pEmitterTable);
        if(pHandler && !pHandler->blocked) {
            GblDoublyLinkedListNode* pNode = GblDoublyLinkedList_front(&pHandler->connectionList);

            // Don't bother setting up stack frame if there's no connections
            if(pNode) {
                Connection_*    pConnection = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode, Connection_, emitterList);
                const size_t    argCount    = pConnection->pSignal->argCount + 1;
                GblVariant*     pArgValues  = GBL_ALLOCA(sizeof(GblVariant) * argCount);

                // initialize argument values
                GBL_CTX_VERIFY_CALL(GblVariant_constructPointer(&pArgValues[0], GBL_POINTER_TYPE, pConnection->pReceiver));
                if(pVarArgs) {
                    for(size_t  a = 1; a < argCount; ++a) {
                        GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopyVa(&pArgValues[a],
                                                                                pConnection->pSignal->argTypes[a-1],
                                                                                pVarArgs));
                    }
                } else if(pVariantArgs) {
                    for(size_t  a = 1; a < argCount; ++a) {
                        GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&pArgValues[a],
                                                                     &pVariantArgs[a-1]));
                    }
                } else GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION);

                // call closures
                GblBool firstConnection = GBL_TRUE;
                do {
                    pConnection = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode, Connection_, emitterList);

                    // update arg[0]: receiver (already set for first connection, though)
                    if(!firstConnection)
                        GBL_CTX_VERIFY_CALL(GblVariant_setPointer(&pArgValues[0], GBL_POINTER_TYPE, pConnection->pReceiver));
                    else
                        firstConnection = GBL_FALSE;

                    // save current active connection state
                    Connection_* pOldConnection = pActiveConnection_;
                    pActiveConnection_ = pConnection;

                    // invoke closure
                    GBL_CTX_VERIFY_CALL(GblClosure_invoke(pConnection->pClosure,
                                                          NULL,
                                                          argCount,
                                                          pArgValues));
                    // restore active connection state
                    pActiveConnection_ = pOldConnection;

                } while((pNode = pNode->pNext) != &pHandler->connectionList);

                // destruct arguments
                for(size_t  a = 0; a < argCount; ++a) {
                    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&pArgValues[a]));
                }
            }
        }
    }

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblSignal_emitVariants(GblInstance* pEmitter,
                                             const char*  pSignalName,
                                             GblVariant*  pVariantArgs)
{
    return GblSignal_emit_(pEmitter, pSignalName, NULL, pVariantArgs);

}

GBL_EXPORT GBL_RESULT GblSignal_emitVaList(GblInstance* pEmitter,
                                           const char*  pSignalName,
                                           va_list*     pVarArgs)
{
    return GblSignal_emit_(pEmitter, pSignalName, pVarArgs, NULL);
}

GBL_EXPORT GBL_RESULT GblSignal_emit(GblInstance* pEmitter,
                                     const char*  pSignalName,
                                     ...)
{
    va_list varArgs;
    va_start(varArgs, pSignalName);
    GBL_RESULT result = GblSignal_emitVaList(pEmitter, pSignalName, &varArgs);
    va_end(varArgs);
    return result;
}

GBL_EXPORT GblInstance* GblSignal_emitter(void) {
    return pActiveConnection_? pActiveConnection_->pEmitter : NULL;
}

GBL_EXPORT GblInstance* GblSignal_receiver(void) {
    return pActiveConnection_? pActiveConnection_->pReceiver : NULL;
}

// Fix for Windows: "Redefinition with different linkage".
// GBL_EXPORT moved to declaration in gimbal_type_.h
GBL_RESULT GblSignal_init_(GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblHashSet_construct(&signalSet_,
                                             sizeof(Signal_*),
                                             signalSetHasher_,
                                             signalSetComparator_,
                                             signalSetDestructor_,
                                             64,
                                             pCtx));

    GBL_CTX_VERIFY_CALL(GblHashSet_construct(&instanceConnectionTableSet_,
                                             sizeof(InstanceConnectionTable_*),
                                             instanceConnectionTableHasher_,
                                             instanceConnectionTableComparator_,
                                             instanceConnectionTableDestructor_,
                                             0,
                                             pCtx));

#ifdef GBL_CONNECTION_POOL_ALLOCATOR_
        GBL_CTX_VERIFY_CALL(GblPoolAllocator_construct(&connectionAllocator_,
                                                       sizeof(Connection_),
                                                       GBL_CONNECTION_POOL_ALLOCATOR_PAGE_SIZE_));
#endif

    GBL_CTX_END();
}

// Fix for Windows: "Redefinition with different linkage".
// GBL_EXPORT moved to declaration in gimbal_type_.h
GBL_RESULT GblSignal_final_(GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_CALL(GblHashSet_destruct(&signalSet_));
    GBL_CTX_CALL(GblHashSet_destruct(&instanceConnectionTableSet_));
#ifdef GBL_CONNECTION_POOL_ALLOCATOR_
    GBL_CTX_CALL(GblPoolAllocator_destruct(&connectionAllocator_));
#endif
    GBL_CTX_END();
}
