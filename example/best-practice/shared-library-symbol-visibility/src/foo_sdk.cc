#include "foo_sdk.h"
#include "foo_sdk_internal.h"

#include <cstring>
#include <new>
#include <string>

extern "C" const char* foo_sdk_version(void) {
    return "1.2.0";
}

extern "C" FooSdkStatus foo_sdk_create(const FooSdkConfig* config,
                                        FooSdkHandle** handle) {
    if (config == nullptr || handle == nullptr || config->endpoint == nullptr) {
        return FOO_SDK_INVALID_ARGUMENT;
    }

    const std::string endpoint =
        foo_sdk_internal::normalize_endpoint(config->endpoint);
    if (!foo_sdk_internal::is_valid_endpoint(endpoint)) {
        return FOO_SDK_INVALID_ARGUMENT;
    }

    FooSdkHandle* instance = new (std::nothrow) FooSdkHandle;
    if (instance == nullptr) {
        return FOO_SDK_INTERNAL_ERROR;
    }

    instance->endpoint = endpoint;
    instance->timeout_ms = config->timeout_ms;
    instance->request_count = 0;
    instance->diagnostics_enabled = config->enable_diagnostics != 0;
    *handle = instance;
    return FOO_SDK_OK;
}

extern "C" void foo_sdk_destroy(FooSdkHandle* handle) {
    delete handle;
}

extern "C" FooSdkStatus foo_sdk_send(FooSdkHandle* handle,
                                      const char* request,
                                      char* response,
                                      size_t response_capacity) {
    if (handle == nullptr) {
        return FOO_SDK_NOT_INITIALIZED;
    }
    if (request == nullptr || response == nullptr) {
        handle->last_error = "request and response must not be null";
        return FOO_SDK_INVALID_ARGUMENT;
    }

    foo_sdk_internal::trace_request(*handle, request);
    const std::string result = foo_sdk_internal::make_response(
        handle->endpoint, request, ++handle->request_count);
    if (result.size() + 1 > response_capacity) {
        handle->last_error = "response buffer is too small";
        return FOO_SDK_BUFFER_TOO_SMALL;
    }

    std::memcpy(response, result.c_str(), result.size() + 1);
    handle->last_error.clear();
    return FOO_SDK_OK;
}

extern "C" const char* foo_sdk_last_error(const FooSdkHandle* handle) {
    return handle == nullptr ? "SDK is not initialized" : handle->last_error.c_str();
}
