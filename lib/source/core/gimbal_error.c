#include <stdio.h>
#include <stdlib.h>
#include <gimbal/core/gimbal_tls.h>
#include <gimbal/core/gimbal_error.h>

static GBL_TLS(GblError, currentError_, { 0 });

static const char* gblErrorCodeStr_(GblEnum code) {
    return gblResultString(code);
}

GBL_EXPORT const GblErrorDomain* gblErrorDomain(void) {
    static GblErrorDomain domain;
    static GblBool once = GBL_FALSE;

    if(!once) {
        domain.name          = GblQuark_fromStatic("GblError");
        domain.pFnCodeString = gblErrorCodeStr_;

        once = GBL_TRUE;
    }

    return &domain;
}

GBL_EXPORT const GblError* GblError_pending(void) {
    GblError* pError = GBL_TLS_LOAD(currentError_);

    return (pError && pError->pDomain) ? pError : NULL;
}

GBL_EXPORT GblQuark GblError_domain(void) {
    GblQuark        quark  = GBL_QUARK_INVALID;
    const GblError* pError = GblError_pending();

    if(pError) {
        quark = pError->pDomain->name;
    }

    return quark;
}

GBL_EXPORT const char* GblError_string(void) {
    const char*     pStr   = "";
    const GblError* pError = GblError_pending();

    if(pError) {
        pStr = pError->pDomain->pFnCodeString(pError->code);
    }

    return pStr;
}

GBL_EXPORT GblBool GblError_clear(void) {
    GblBool   cleared = GBL_FALSE;
    GblError* pError  = GBL_TLS_LOAD(currentError_);

    if(pError && pError->pDomain) {
        pError->pDomain = NULL;
        cleared         = GBL_TRUE;
    }

    return cleared;
}

GBL_EXPORT const GblError* GblError_raisev(const char*           pFile,
                                           const char*           pFunction,
                                           size_t                line,
                                           const GblErrorDomain* pDomain,
                                           GblEnum               code,
                                           const char*           pFmt,
                                           va_list               varArgs)
{
    GblError* pError = GBL_TLS_LOAD(currentError_);

    pError->srcLocation.pFile = pFile;
    pError->srcLocation.pFunc = pFunction;
    pError->srcLocation.line  = line;
    pError->pDomain           = pDomain;
    pError->code              = code;

    if(pFmt)
        vsnprintf(pError->message, sizeof(pError->message), pFmt, varArgs);
    else
        pError->message[0] = '\0';

    return pError;
}

GBL_EXPORT const GblError* (GblError_raise)(const char*           pFile,
                                            const char*           pFunction,
                                            size_t                line,
                                            const GblErrorDomain* pDomain,
                                            GblEnum               code,
                                            const char*           pFmt,
                                            ...)
{
    va_list varArgs;
    va_start(varArgs, pFmt);

    const GblError* pError =
            GblError_raisev(pFile,
                            pFunction,
                            line,
                            pDomain,
                            code,
                            pFmt,
                            varArgs);
    va_end(varArgs);

    return pError;
}

GBL_EXPORT const GblError* (GblError_reraise)(const char* pFile,
                                              const char* pFunction,
                                              size_t      line)
{
    GblError* pError = GBL_TLS_LOAD(currentError_);

    GBL_ASSERT(pError->pDomain, "Cannot reraise without existing raised GblError!");

    pError->srcLocation.pFile = pFile;
    pError->srcLocation.pFunc = pFunction;
    pError->srcLocation.line  = line;

    return pError;
}
