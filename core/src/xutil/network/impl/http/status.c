/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "http_status"
#define XU_TRACE_MODULE_DEBUG (1)

#include "xutil/network/impl/http/status.h"

/* *******************************************************
 * implementation
 */
xu_bool_t xu_http_status_init(xu_http_status_t* status)
{
    // check
    xu_assert_and_check_return_val(status, xu_false);

    // init status using the default value
    status->version = 1;

    // init content type
    if (!xu_string_init(&status->content_type)) return xu_false;

    // init location
    if (!xu_string_init(&status->location)) return xu_false;

    // ok
    return xu_true;
}
xu_void_t xu_http_status_exit(xu_http_status_t* status)
{
    // check
    xu_assert_and_check_return(status);

    // exit the content type
    xu_string_exit(&status->content_type);

    // exit location
    xu_string_exit(&status->location);
}
xu_void_t xu_http_status_cler(xu_http_status_t* status, xu_bool_t host_changed)
{
    // check
    xu_assert_and_check_return(status);

    // clear status
    status->code          = 0;
    status->bgzip         = 0;
    status->bdeflate      = 0;
    status->bchunked      = 0;
    status->content_size  = -1;
    status->document_size = -1;
    status->state         = XU_STATE_OK;

    // clear content type
    xu_string_clear(&status->content_type);

    // clear location
    xu_string_clear(&status->location);

    // host is changed? clear the alived state
    if (host_changed)
    {
        status->version = 1;
        status->balived = 0;
        status->bseeked = 0;
    }
}

#ifdef __xu_debug__
xu_void_t xu_http_status_dump(xu_http_status_t* status)
{
    // check
    xu_assert_and_check_return(status);

    // dump status
    xu_trace_i("======================================================================");
    xu_trace_i("status: ");
    xu_trace_i("status: code: %d", status->code);
    xu_trace_i("status: version: HTTP/1.%1u", status->version);
    xu_trace_i("status: content:type: %s", xu_string_cstr(&status->content_type));
    xu_trace_i("status: content:size: %lld", status->content_size);
    xu_trace_i("status: document:size: %lld", status->document_size);
    xu_trace_i("status: location: %s", xu_string_cstr(&status->location));
    xu_trace_i("status: bgzip: %s", status->bgzip ? "true" : "false");
    xu_trace_i("status: bdeflate: %s", status->bdeflate ? "true" : "false");
    xu_trace_i("status: balived: %s", status->balived ? "true" : "false");
    xu_trace_i("status: bseeked: %s", status->bseeked ? "true" : "false");
    xu_trace_i("status: bchunked: %s", status->bchunked ? "true" : "false");

    // dump end
    xu_trace_i("");
}
#endif
