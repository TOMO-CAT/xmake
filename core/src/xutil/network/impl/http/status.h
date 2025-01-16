#pragma once

#include "xutil/network/http.h"
#include "xutil/prefix.h"
#include "xutil/utils/state.h"

/* init status
 *
 * @param status        the status
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_http_status_init(xu_http_status_t* status);

/* exit status
 *
 * @param status        the status
 */
xu_void_t xu_http_status_exit(xu_http_status_t* status);

/* clear status
 *
 * @param status        the status
 * @param host_changed  the host is changed
 */
xu_void_t xu_http_status_cler(xu_http_status_t* status, xu_bool_t host_changed);

#ifdef __xu_debug__
/* dump status
 *
 * @param status        the status
 */
xu_void_t xu_http_status_dump(xu_http_status_t* status);
#endif
