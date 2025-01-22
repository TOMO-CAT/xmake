#pragma once

#include "xutil/config.h"

/* *******************************************************
 *  macros
 */
// the major version
#define XU_VERSION_MAJOR XU_CONFIG_VERSION_MAJOR

// the minor version
#define XU_VERSION_MINOR XU_CONFIG_VERSION_MINOR

// the alter version
#define XU_VERSION_ALTER XU_CONFIG_VERSION_ALTER

// the build version
#ifndef XU_CONFIG_VERSION_BUILD
#    define XU_CONFIG_VERSION_BUILD 0
#endif
#define XU_VERSION_BUILD XU_CONFIG_VERSION_BUILD

// the build version string
#define XU_VERSION_BUILD_STRING __xu_mstring_ex__(XU_CONFIG_VERSION_BUILD)

// the version string
#define XU_VERSION_STRING                                                                                              \
    __xu_mstrcat6__("xutil_",                                                                                          \
                    __xu_mstring_ex__(__xu_mconcat8_ex__(v, XU_VERSION_MAJOR, _, XU_VERSION_MINOR, _,                  \
                                                         XU_VERSION_ALTER, _, XU_CONFIG_VERSION_BUILD)),               \
                    "_", XU_ARCH_VERSION_STRING, " by ", XU_COMPILER_VERSION_STRING)

// the short version string
#define XU_VERSION_SHORT_STRING                                                                                        \
    __xu_mstrcat__("xutil_", __xu_mstring_ex__(__xu_mconcat8_ex__(v, XU_VERSION_MAJOR, _, XU_VERSION_MINOR, _,         \
                                                                  XU_VERSION_ALTER, _, XU_CONFIG_VERSION_BUILD)))
