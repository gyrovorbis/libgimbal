#include <gimbal/core/gimbal_logger.h>
#include <tinycthread.h>

#define GBL_LOGGER_THREAD_FILTER_FIELD_NAME_    "GblLogger.threadFilter"
#define GBL_LOGGER_DOMAIN_FILTER_FIELD_NAME_    "GblLogger.domainFilter"
#define GBL_LOGGER_ENTRY_(node)                 GBL_LINKED_LIST_ENTRY(node, GblLogger, private_.listNode)
#define GBL_LOGGER_ENSURE_INITIALIZED_()                      \
    GBL_STMT_START {                                          \
        if(!initializing_) {                                  \
            if(!inittedOnce_)                                 \
                call_once(&initOnce_, GblLogger_initialize_); \
            else if(!initialized_) GblLogger_initialize_();   \
        }                                                     \
    } GBL_STMT_END


static GblLinkedListNode loggerList_ = {
    .pNext = &loggerList_
};

static mtx_t     listMtx_;
static GblBool   inittedOnce_       = GBL_FALSE;
static GblBool   initialized_       = GBL_FALSE;
static GblBool   initializing_      = GBL_FALSE;
static once_flag initOnce_          = ONCE_FLAG_INIT;
static GblQuark  domainFilterQuark_ = GBL_QUARK_INVALID;
static GblQuark  threadFilterQuark_ = GBL_QUARK_INVALID;
static GBL_THREAD_LOCAL
       size_t    stackDepth_        = 0;

static void GblLogger_finalize_(void) {
    if(!initialized_) return;
    mtx_lock(&listMtx_);

    for(GblLinkedListNode* pIt = loggerList_.pNext;
        pIt != &loggerList_;
        pIt = pIt->pNext) {
            GblBox_unref(GBL_BOX(GBL_LOGGER_ENTRY_(pIt)));
    }

    loggerList_.pNext = &loggerList_;
    initialized_ = GBL_FALSE;

    mtx_unlock(&listMtx_);
}

static void GblLogger_initialize_(void) {
    if(!inittedOnce_) mtx_init(&listMtx_, mtx_recursive);
    mtx_lock(&listMtx_);
    inittedOnce_ = GBL_TRUE;
    initializing_ = GBL_TRUE;

    stackDepth_        = 0;
    domainFilterQuark_ = GblQuark_fromStringStatic(GBL_LOGGER_DOMAIN_FILTER_FIELD_NAME_);
    threadFilterQuark_ = GblQuark_fromStringStatic(GBL_LOGGER_THREAD_FILTER_FIELD_NAME_);

    initializing_ = GBL_FALSE;
    initialized_ = GBL_TRUE;
    atexit(GblLogger_finalize_);
    mtx_unlock(&listMtx_);
}

GBL_EXPORT GBL_RESULT GblLogger_register(GblLogger* pLogger) {
    GBL_CTX_BEGIN(NULL);
    GBL_LOGGER_ENSURE_INITIALIZED_();
    mtx_lock(&listMtx_);

    GblBox_ref(GBL_BOX(pLogger));

    GblLinkedList_pushFront(&loggerList_,
                            &GBL_PRIV_REF(pLogger).listNode);


    GBL_CTX_END_BLOCK();
    mtx_unlock(&listMtx_);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblLogger_unregister(GblLogger* pLogger) {
    GBL_CTX_BEGIN(NULL);
    GBL_LOGGER_ENSURE_INITIALIZED_();
    mtx_lock(&listMtx_);

    GblLinkedList_remove(&loggerList_,
                        &GBL_PRIV_REF(pLogger).listNode);

    GblBox_unref(GBL_BOX(pLogger));

    GBL_CTX_END_BLOCK();
    mtx_unlock(&listMtx_);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GblBool GblLogger_foreach(GblLoggerIterFn pFnIt,
                                     void* pClosure) {
    GblBool result = GBL_FALSE;

    GBL_CTX_BEGIN(NULL);
    GBL_LOGGER_ENSURE_INITIALIZED_();
    mtx_lock(&listMtx_);

    for(GblLinkedListNode* pIt = loggerList_.pNext;
        pIt != &loggerList_;
        pIt = pIt->pNext) {
        if(pFnIt(GBL_LOGGER_ENTRY_(pIt), pClosure)) {
            result = GBL_TRUE;
            break;
        }
    }

    GBL_CTX_END_BLOCK();
    mtx_unlock(&listMtx_);
    return result;
}

GBL_EXPORT GblLogger* GblLogger_create(GblType         derived,
                                       size_t          allocSize,
                                       GblLoggerClass* pClass) {
    GblLogger* pSelf = NULL;
    GBL_CTX_BEGIN(NULL);

    GBL_ASSERT(allocSize >= sizeof(GblLogger));

    pSelf = GBL_CTX_MALLOC(allocSize);

    GBL_CTX_VERIFY_CALL(GblLogger_construct(pSelf,
                                            derived,
                                            pClass));

    GBL_CTX_END_BLOCK();
    return pSelf;
}

GBL_EXPORT GBL_RESULT GblLogger_construct(GblLogger* pSelf,
                                          GblType derived,
                                          GblLoggerClass* pClass) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(derived, GBL_LOGGER_TYPE);

    if(pClass)
        GBL_CTX_VERIFY_CALL(GblInstance_constructWithClass(GBL_INSTANCE(pSelf),
                                                           GBL_CLASS(pClass)));
    else
        GBL_CTX_VERIFY_CALL(GblInstance_construct(GBL_INSTANCE(pSelf),
                                                  derived));

    GBL_CTX_END();
}

GBL_EXPORT GblRefCount GblLogger_unref(GblLogger* pSelf) {
    return GblBox_unref(GBL_BOX(pSelf));
}


GBL_EXPORT GblBool GblLogger_hasFilter(const GblLogger* pSelf,
                                       const GblThd* pThread,
                                       const char*      pDomain,
                                       GBL_LOG_FLAGS    flags) {
    if(!(pSelf->flagsFilter & flags))
        return GBL_FALSE;

    if(!GblLogger_hasDomainFilter(pSelf, pDomain))
        return GBL_FALSE;

    if(!GblLogger_hasThreadFilter(pSelf, pThread))
        return GBL_FALSE;

    return GBL_TRUE;
}

GBL_EXPORT GblBool GblLogger_hasDomainFilter(const GblLogger* pSelf,
                                             const char* pDomain) {
    const char** ppDomains = GblLogger_domainFilters(pSelf);
    if(ppDomains) {
        while(*ppDomains) {
            if(strcmp(*ppDomains++, pDomain) == 0) {
                return GBL_TRUE;
            }
        }
        return GBL_FALSE;
    } else return GBL_TRUE;
}

GBL_RESULT domainFilterDtor_(const GblArrayMap* pMap, uintptr_t key, void* pEntry) {
    GBL_UNUSED(pMap, key);
    GBL_CTX_BEGIN(NULL);
    char** ppStrings = pEntry;
    while(*ppStrings) {
        GBL_CTX_FREE(*ppStrings);
        ppStrings++;
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblLogger_setDomainFilters(GblLogger* pSelf,
                                                 const char* pDomains[]) {
    GBL_CTX_BEGIN(NULL);

    size_t  count = 0;
    while(pDomains[count++]);

    // don't bother if it's just an empty delimeter list
    if(count == 1) GBL_CTX_DONE();

    char** ppDomains = GBL_CTX_NEW(char*, count);
    for(size_t  s = 0; s < count-1; ++s) {
        ppDomains[s] = GBL_CTX_NEW(char, strlen(pDomains[s]));
        strcpy(ppDomains[s], pDomains[s]);
    }
    ppDomains[count-1] = NULL;

    GBL_CTX_VERIFY_CALL(GblBox_setField(GBL_BOX(pSelf),
                                        domainFilterQuark_,
                                        (uintptr_t)ppDomains,
                                        domainFilterDtor_));
    GBL_CTX_END();
}

GBL_EXPORT const char** GblLogger_domainFilters(const GblLogger* pSelf) {
    return (const char**)GblBox_getField(GBL_BOX(pSelf),
                                         domainFilterQuark_);
}

GBL_EXPORT GblBool GblLogger_hasThreadFilter(const GblLogger* pSelf,
                                             const GblThd* pThread) {
    const GblThd** ppThreads = GblLogger_threadFilters(pSelf);
    if(ppThreads) {
        while(*ppThreads) {
            if(*ppThreads++ == pThread) {
                return GBL_TRUE;
            }
        }
        return GBL_FALSE;
    } else return GBL_TRUE;
}

GBL_RESULT threadFilterDtor_(const GblArrayMap* pMap, uintptr_t key, void* pEntry) {
    GBL_UNUSED(pMap, key);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_FREE(pEntry);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblLogger_setThreadFilters(GblLogger* pSelf,
                                                 const GblThd* pThreads[]) {
    GBL_CTX_BEGIN(NULL);

    size_t  count = 0;
    while(pThreads[count++]);

    // Don't bother if it's just an empty delimeter list
    if(count == 1) GBL_CTX_DONE();

    const GblThd** ppThreads = GBL_CTX_NEW(const GblThd*, count);
    memcpy(ppThreads, pThreads, sizeof(GblThd*) * count);

    // Cram the bitch into the box
    GBL_CTX_VERIFY_CALL(GblBox_setField(GBL_BOX(pSelf),
                                        threadFilterQuark_,
                                        (uintptr_t)ppThreads,
                                        threadFilterDtor_));
    GBL_CTX_END();
}

GBL_EXPORT const GblThd** GblLogger_threadFilters(const GblLogger* pSelf) {
    return (const GblThd**)GblBox_getField(GBL_BOX(pSelf),
                                              threadFilterQuark_);
}


GBL_EXPORT GBL_RESULT GblLogger_push(void) {
    GBL_CTX_BEGIN(NULL);
    GBL_LOGGER_ENSURE_INITIALIZED_();
    mtx_lock(&listMtx_);

    ++stackDepth_;

    GblThd* pThread = GblThd_current();

    for(GblLinkedListNode* pIt = loggerList_.pNext;
        pIt != &loggerList_;
        pIt = pIt->pNext)
    {
        GblLogger* pLogger = GBL_LOGGER_ENTRY_(pIt);
        if(GblLogger_hasThreadFilter(pLogger, pThread)) {
            GBL_CTX_CALL(GBL_LOGGER_GET_CLASS(pLogger)->pFnPush(pLogger, pThread));
        }
    }

    GBL_CTX_END_BLOCK();
    mtx_unlock(&listMtx_);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblLogger_pop(size_t  count) {

    GBL_CTX_BEGIN(NULL);
    GBL_LOGGER_ENSURE_INITIALIZED_();
    mtx_lock(&listMtx_);

    GBL_ASSERT(stackDepth_, "Underflowed log stack for thread!");
    --stackDepth_;

    GblThd* pThread = GblThd_current();

    for(GblLinkedListNode* pIt = loggerList_.pNext;
        pIt != &loggerList_;
        pIt = pIt->pNext)
    {
        GblLogger* pLogger = GBL_LOGGER_ENTRY_(pIt);
        if(GblLogger_hasThreadFilter(pLogger, pThread)) {
            GBL_CTX_CALL(GBL_LOGGER_GET_CLASS(pLogger)->pFnPop(pLogger,
                                                               pThread,
                                                               count));
        }
    }

    GBL_CTX_END_BLOCK();
    mtx_unlock(&listMtx_);
    return GBL_CTX_RESULT();

}

GBL_EXPORT GBL_RESULT GblLogger_write(const char*   pFile,
                                      const char*   pFunction,
                                      size_t        line,
                                      const char*   pDomain,
                                      GBL_LOG_FLAGS flags,
                                      const char*   pFmt,
                                      ...) {
    va_list varArgs;
    va_start(varArgs, pFmt);

    GBL_RESULT retVal = GblLogger_writeVa(pFile,
                                          pFunction,
                                          line,
                                          pDomain,
                                          flags,
                                          pFmt,
                                          varArgs);
    va_end(varArgs);
    return retVal;

}

GBL_EXPORT GBL_RESULT GblLogger_writeVa(const char*   pFile,
                                        const char*   pFunction,
                                        size_t        line,
                                        const char*   pDomain,
                                        GBL_LOG_FLAGS flags,
                                        const char*   pFmt,
                                        va_list       varArgs) {
    GBL_CTX_BEGIN(NULL);
    GBL_LOGGER_ENSURE_INITIALIZED_();
    mtx_lock(&listMtx_);

    GblThd* pThread = GblThd_current();
    const time_t timeStamp = time(NULL);

    for(GblLinkedListNode* pIt = loggerList_.pNext;
        pIt != &loggerList_;
        pIt = pIt->pNext)
    {
        GblLogger* pLogger = GBL_LOGGER_ENTRY_(pIt);
        if(GblLogger_hasFilter(pLogger, pThread, pDomain, flags)) {
            const GblBool firstTime = !GBL_PRIV_REF(pLogger).reentrant;

            if(!firstTime &&
               !(pLogger->flagsFilter & GBL_LOG_REENTRANT)) {
                GBL_ASSERT(GBL_FALSE, "Logger doesn't support reentrancy!");
                continue;
            } else GBL_PRIV_REF(pLogger).reentrant = GBL_TRUE;

            GBL_CTX_CALL(GBL_LOGGER_GET_CLASS(pLogger)->pFnWrite(pLogger,
                                                                 pFile,
                                                                 pFunction,
                                                                 line,
                                                                 pThread,
                                                                 timeStamp,
                                                                 pDomain,
                                                                 flags,
                                                                 pFmt,
                                                                 varArgs));

            if(firstTime)
                GBL_PRIV_REF(pLogger).reentrant = GBL_FALSE;

        }
    }

    GBL_CTX_END_BLOCK();
    mtx_unlock(&listMtx_);
    return GBL_CTX_RESULT();
}


static GBL_RESULT GblLogger_push_(GblLogger* pSelf, GblThd* pThread) {
    GBL_UNUSED(pSelf, pThread);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblLogger_pop_(GblLogger* pSelf, GblThd* pThread, size_t  count) {
    GBL_UNUSED(pSelf, pThread, count);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblLogger_write_(GblLogger*    pSelf,
                                   const char*   pFile,
                                   const char*   pFunction,
                                   size_t        line,
                                   GblThd*    pThread,
                                   time_t        timeStamp,
                                   const char*   pDomain,
                                   GBL_LOG_FLAGS flags,
                                   const char*   pFmt,
                                   va_list       varArgs) {
    GBL_UNUSED(pThread, timeStamp);
    GBL_ASSERT(pFmt);

    // Uh, we don't handle untagged crap
    if(!(pSelf->flagsFilter & flags)) return GBL_RESULT_SUCCESS;

    char buffer[GBL_VA_SNPRINTF_BUFFER_SIZE];
    char tabBuff[GBL_VA_SNPRINTF_BUFFER_SIZE];
    // Route errors to stderr
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    FILE* const pOut = (flags & GBL_LOG_ERROR)?
                            stderr : stdout;
    const char* pPrefix = NULL;

    // Add prefix for most severe flag
         if(flags & GBL_LOG_ERROR)   pPrefix = "X ";
    else if(flags & GBL_LOG_WARN) pPrefix = "! ";
    else if(flags & GBL_LOG_INFO)    pPrefix = "* ";
    else if(flags & GBL_LOG_DEBUG)   pPrefix = "# ";
    else                             pPrefix = "";

    // populate formatted string buffer
    const int vsnprintfBytes = vsnprintf(buffer, sizeof(buffer), pFmt, varArgs);

    // check for truncation
    if(vsnprintfBytes > (int)sizeof(buffer)) {
        pPrefix = "T - "; //Truncated prefix!
        result = GBL_RESULT_TRUNCATED;
    }

    // Create tab buffer from stack depth
    if(pSelf) {
        for(unsigned t = 0; t < stackDepth_ * 8; ++t) {
            tabBuff[t] = ' ';
        }
        tabBuff[stackDepth_ * 8] = '\0';
    } else {
        tabBuff[0] = '\0';
    }

    // Print the message with extra debug info for errors and warnings
    if((flags & GBL_LOG_ERROR) || (flags & GBL_LOG_WARN)) {
        if((fprintf(pOut, "[%6s] %s%s%s\n[%6s] %s        @ %s(..): %s:%zu\n",
                    pDomain,
                    tabBuff, pPrefix, buffer,
                    pDomain, tabBuff,
                    pFunction,
                    pFile,
                    line)
            < 0)) result = GBL_RESULT_ERROR_FILE_WRITE;
    } else {// Print the message regularly
        if(((fprintf(pOut, "[%6s] %s%s%s\n",
                    pDomain,
                    tabBuff, pPrefix, buffer)
            >= 0))) result = GBL_RESULT_ERROR_FILE_WRITE;
    }

    // Flush the buffer to not miss anything
    if(fflush(pOut) == 0) result = GBL_RESULT_ERROR_FILE_WRITE;

    return result;
}

static GBL_RESULT GblLogger_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_UNUSED(pCtx);
    GBL_CTX_BEGIN(NULL);

    GBL_LOGGER(pInstance)->flagsFilter = ~(0x0);

    GBL_CTX_END();
}

static GBL_RESULT GblLogger_GblObject_setProperty(GblObject* pObject,
                                                  const GblProperty* pProp,
                                                  GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblLogger* pSelf = GBL_LOGGER(pObject);

    switch(pProp->id) {
    case GblLogger_Property_Id_domainFilters:
        GblLogger_setDomainFilters(pSelf, GblVariant_getPointer(pValue));
        break;
    case GblLogger_Property_Id_flagsFilter:
        pSelf->flagsFilter = GblVariant_getFlags(pValue);
        break;
    case GblLogger_Property_Id_threadFilters:
        GblLogger_setThreadFilters(pSelf, GblVariant_getPointer(pValue));
        break;
    default: break;
    }

    GBL_CTX_END();
}

static GBL_RESULT GblLogger_GblObject_property(const GblObject* pObject,
                                               const GblProperty* pProp,
                                               GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblLogger* pSelf = GBL_LOGGER(pObject);

    switch(pProp->id) {
    case GblLogger_Property_Id_domainFilters:
        GblVariant_setPointer(pValue, GBL_POINTER_TYPE, (void*)GblLogger_domainFilters(pSelf));
        break;
    case GblLogger_Property_Id_flagsFilter:
        GblVariant_setFlags(pValue, GBL_FLAGS_TYPE, pSelf->flagsFilter);
        break;
    case GblLogger_Property_Id_threadFilters:
        GblVariant_setPointer(pValue, GBL_POINTER_TYPE, (void*)GblLogger_threadFilters(pSelf));
        break;
    default: break;
    }

    GBL_CTX_END();
}

static GBL_RESULT GblLoggerClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd, pCtx);
    GBL_CTX_BEGIN(NULL);

    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GblLogger_GblObject_property;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GblLogger_GblObject_setProperty;
    GBL_LOGGER_CLASS(pClass)->pFnPush        = GblLogger_push_;
    GBL_LOGGER_CLASS(pClass)->pFnPop         = GblLogger_pop_;
    GBL_LOGGER_CLASS(pClass)->pFnWrite       = GblLogger_write_;

    GBL_CTX_END();
}

GBL_EXPORT GblType GblLogger_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo typeInfo = {
        .classSize          = sizeof(GblLoggerClass),
        .pFnClassInit       = GblLoggerClass_init_,
        .instanceSize       = sizeof(GblLogger),
        .pFnInstanceInit    = GblLogger_init_,
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        GBL_CTX_BEGIN(NULL);

        type = GblType_registerStatic(GblQuark_internStringStatic("GblLogger"),
                                      GBL_OBJECT_TYPE,
                                      &typeInfo,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);

        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}
