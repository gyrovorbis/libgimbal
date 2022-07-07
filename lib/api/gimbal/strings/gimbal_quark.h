#ifndef GIMBAL_QUARK_H
#define GIMBAL_QUARK_H

#include <gimbal/types/gimbal_typedefs.h>

GBL_DECLS_BEGIN

typedef uintptr_t         GblQuark;

#define GBL_QUARK_INVALID ((GblQuark)0)

GBL_EXPORT GBL_RESULT   GblQuark_init               (GblContext* pCtx,
                                                     GblSize extraPageSize,
                                                     GblSize initialEntries)                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblQuark_final              (void)                                  GBL_NOEXCEPT;

GBL_EXPORT GblSize      GblQuark_pageCount          (void)                                  GBL_NOEXCEPT;
GBL_EXPORT GblSize      GblQuark_pageSize           (void)                                  GBL_NOEXCEPT;
GBL_EXPORT GblSize      GblQuark_bytesUsed          (void)                                  GBL_NOEXCEPT;
GBL_EXPORT GblSize      GblQuark_count              (void)                                  GBL_NOEXCEPT;
GBL_EXPORT GblContext*  GblQuark_context            (void)                                  GBL_NOEXCEPT;

GBL_EXPORT GblQuark     GblQuark_fromString         (const char* pString)                   GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblQuark_fromStringSized    (const char* pString, GblSize length)   GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblQuark_fromStringStatic   (const char* pSstring)                  GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblQuark_tryString          (const char* pString)                   GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblQuark_tryStringSized     (const char* pString, GblSize length)   GBL_NOEXCEPT;

GBL_EXPORT const char*  GblQuark_toString           (GblQuark quark)                        GBL_NOEXCEPT;

GBL_EXPORT const char*  GblQuark_internString       (const char* pString)                   GBL_NOEXCEPT;
GBL_EXPORT const char*  GblQuark_internStringSized  (const char* pString, GblSize length)   GBL_NOEXCEPT;
GBL_EXPORT const char*  GblQuark_internStringStatic (const char* pString)                   GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_QUARK_H
