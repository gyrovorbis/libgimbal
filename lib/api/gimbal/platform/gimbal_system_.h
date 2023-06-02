#ifndef GIMBAL_SYSTEM__H
#define GIMBAL_SYSTEM__H

GBL_EXPORT size_t      GblSystem__cpuCount     (void) GBL_NOEXCEPT;
GBL_EXPORT size_t      GblSystem__totalRam     (void) GBL_NOEXCEPT;
GBL_EXPORT size_t      GblSystem__availableRam (void) GBL_NOEXCEPT;

GBL_EXPORT const char* GblSystem__osName       (void) GBL_NOEXCEPT;
GBL_EXPORT const char* GblSystem__osVersion    (void) GBL_NOEXCEPT;
GBL_EXPORT const char* GblSystem__hostName     (void) GBL_NOEXCEPT:
GBL_EXPORT const char* GblSystem__architecture (void) GBL_NOEXCEPT;


#endif // GIMBAL_SYSTEM__H
