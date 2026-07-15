#ifndef FOO_SDK_INTERNAL_H
#define FOO_SDK_INTERNAL_H

#include <stdint.h>

#include <string>

struct FooSdkHandle {
    std::string endpoint;
    std::string last_error;
    uint32_t timeout_ms;
    uint64_t request_count;
    bool diagnostics_enabled;
};

namespace foo_sdk_internal {

bool is_valid_endpoint(const std::string& endpoint);
std::string normalize_endpoint(const std::string& endpoint);
std::string make_response(const std::string& endpoint,
                          const std::string& request,
                          uint64_t request_id);
void trace_request(const FooSdkHandle& handle, const std::string& request);

} // namespace foo_sdk_internal

#endif
