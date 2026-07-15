#include "foo_sdk_internal.h"

#include <cstdio>
#include <sstream>

namespace foo_sdk_internal {

bool is_valid_endpoint(const std::string& endpoint) {
    return endpoint.compare(0, 7, "http://") == 0 ||
           endpoint.compare(0, 8, "https://") == 0;
}

std::string normalize_endpoint(const std::string& endpoint) {
    if (!endpoint.empty() && endpoint[endpoint.size() - 1] == '/') {
        return endpoint.substr(0, endpoint.size() - 1);
    }
    return endpoint;
}

std::string make_response(const std::string& endpoint,
                          const std::string& request,
                          uint64_t request_id) {
    std::ostringstream output;
    output << "{\"request_id\":" << request_id
           << ",\"endpoint\":\"" << endpoint
           << "\",\"echo\":\"" << request << "\"}";
    return output.str();
}

void trace_request(const FooSdkHandle& handle, const std::string& request) {
    if (handle.diagnostics_enabled) {
        std::fprintf(stderr, "[foo-sdk] send to %s: %s\n",
                     handle.endpoint.c_str(), request.c_str());
    }
}

} // namespace foo_sdk_internal
