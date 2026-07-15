#ifndef FOO_SDK_TYPES_H
#define FOO_SDK_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FooSdkStatus {
    FOO_SDK_OK = 0,
    FOO_SDK_INVALID_ARGUMENT = 1,
    FOO_SDK_NOT_INITIALIZED = 2,
    FOO_SDK_BUFFER_TOO_SMALL = 3,
    FOO_SDK_INTERNAL_ERROR = 4
} FooSdkStatus;

typedef struct FooSdkConfig {
    const char* endpoint;
    uint32_t timeout_ms;
    int enable_diagnostics;
} FooSdkConfig;

typedef struct FooSdkHandle FooSdkHandle;

#ifdef __cplusplus
}
#endif

#endif
