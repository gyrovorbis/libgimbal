/*! \file
 *  \brief GblClosure base instance and API
 *  \copydoc GblClosure
 *  \ingroup signals
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_CLOSURE_H
#define GIMBAL_CLOSURE_H

#include "../instances/gimbal_box.h"
#include "gimbal_marshal.h"

#define GBL_CLOSURE_TYPE                (GBL_TYPEOF(GblClosure))

#define GBL_CLOSURE(instance)           (GBL_INSTANCE_CAST(instance, GblClosure))
#define GBL_CLOSURE_CLASS(klass)        (GBL_CLASS_CAST(klass, GblClosure))
#define GBL_CLOSURE_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GblClosure))

#ifndef __cplusplus
#   define GBL_CALLBACK(fn)                ((GblFnPtr)fn)
#else
    #define GBL_CALLBACK(fn)               ((GblFnPtr)+fn)
#endif

#define GBL_SELF_TYPE                   GblClosure

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE(GblClosure, GblBox)
    GblMarshalFn    pFnMetaMarshal;
GBL_CLASS_END

/*! \struct GblClosure
 * \brief Base instance for all closure types
 *\ingroup meta
 *\details
 * A GblClosure represents an abstract  generic
 * callback object. It is analogous to a stateful C++
 * function object, or functor, in that it can
 * contain both a callback method as  well as stateful
 * data to operate upon.
 *
 * All closures have a single entry-point for
 * having their callback logic called, which is
 * GblClosure_invoke(). This calls the closure's
 * "marshal" function, which is responsible for
 * - validating arguments and return type
 * - converting arguments into expected format for callback
 * - calling actual callback or logic
 * - converting return value back into a GblVariant
 *
 * As the abstract base closure, there is no underlying
 * language-specific callback state within this class.
 * For calling back into C function pointers, see
 * GblCClosure.
 *
 * \note As GblClosure inherits GblBox, it can
 * contain arbitrary language-specific or
 * binding-specific userdata and has reference
 * count semantics.
 *
 * \sa GblCClosure, GblMarshalFn, gimbal_signal.h
 */
GBL_INSTANCE_DERIVE(GblClosure, GblBox)
    GBL_PRIVATE()
        GblMarshalFn    pFnMarshal;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType     GblClosure_type            (void)                          GBL_NOEXCEPT;
GBL_EXPORT GblClosure* GblClosure_current         (void)                          GBL_NOEXCEPT;
GBL_EXPORT void*       GblClosure_currentUserdata (void)                          GBL_NOEXCEPT;

GBL_EXPORT GblClosure* GblClosure_create          (GblType           derivedType,
                                                   size_t            size,
                                                   void*             pUserdata,
                                                   GblArrayMapDtorFn pFnDtor)     GBL_NOEXCEPT;

GBL_INLINE void        GblClosure_setMarshal      (GBL_SELF,
                                                   GblMarshalFn pFnMarshal)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblClosure_setMetaMarshal  (GBL_SELF,
                                                   GblMarshalFn pFnMeta)          GBL_NOEXCEPT;

GBL_INLINE GblBool     GblClosure_hasMarshal      (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblBool     GblClosure_hasMetaMarshal  (GBL_CSELF)                     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblClosure_invoke          (GBL_SELF,
                                                   GblVariant* pRetValue,
                                                   size_t      argCount,
                                                   GblVariant* pArgValues)        GBL_NOEXCEPT;

// ===== IMPL =====

GBL_INLINE GblBool GblClosure_hasMarshal(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).pFnMarshal? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GblBool GblClosure_hasMetaMarshal(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_CLOSURE_GET_CLASS(pSelf)->pFnMetaMarshal? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE void GblClosure_setMarshal(GBL_SELF, GblMarshalFn pFnMarshal) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).pFnMarshal = pFnMarshal;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_CLOSURE_H
