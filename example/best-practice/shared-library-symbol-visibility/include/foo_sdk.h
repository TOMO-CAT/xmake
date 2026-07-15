#ifndef FOO_SDK_H
#define FOO_SDK_H

#include "foo_sdk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FOO_SDK_VERSION_MAJOR 1
#define FOO_SDK_VERSION_MINOR 2
#define FOO_SDK_VERSION_PATCH 0

const char* foo_sdk_version(void);

FooSdkStatus foo_sdk_create(const FooSdkConfig* config,
                            FooSdkHandle** handle);

void foo_sdk_destroy(FooSdkHandle* handle);

FooSdkStatus foo_sdk_send(FooSdkHandle* handle,
                          const char* request,
                          char* response,
                          size_t response_capacity);

const char* foo_sdk_last_error(const FooSdkHandle* handle);

#ifdef __cplusplus
}
#endif

#endif
