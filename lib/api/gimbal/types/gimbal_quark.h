#ifndef GIMBAL_QUARK_H
#define GIMBAL_QUARK_H

#include <gimbal/types/gimbal_typedefs.h>

GBL_DECLS_BEGIN

#define GBL_QUARK_INVALID ((GblQuark)0)

GBL_API                 gblQuarkReinit(GblContext* pCtx, GblSize pageSize, GblSize initialEntries) GBL_NOEXCEPT;
GblSize                 gblQuarkPageCount(void) GBL_NOEXCEPT;
GblSize                 gblQuarkBytesUsed(void) GBL_NOEXCEPT;
GblSize                 gblQuarkCount(void) GBL_NOEXCEPT;

GBL_EXPORT GblQuark     gblQuarkTryString(const char* pString) GBL_NOEXCEPT;
GBL_EXPORT GblQuark     gblQuarkFromString(const char* pString) GBL_NOEXCEPT;
GBL_EXPORT GblQuark     gblQuarkFromStringStatic(const char* pSstring) GBL_NOEXCEPT;

GBL_EXPORT const char*  gblQuarkToString(GblQuark quark) GBL_NOEXCEPT;
GBL_EXPORT const char*  gblQuarkInternString(const char* pStr) GBL_NOEXCEPT;
GBL_EXPORT const char*  gblQuarkInternStringStatic(const char* pStr) GBL_NOEXCEPT;

GBL_DECLS_END



#endif // GIMBAL_QUARK_H
