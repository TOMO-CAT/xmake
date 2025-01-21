#pragma once

#include "xutil/network/http.h"
#include "xutil/prefix.h"

/* init option
 *
 * @param option        the option
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_http_option_init(xu_http_option_t* option);

/* exit option
 *
 * @param option        the option
 */
xu_void_t xu_http_option_exit(xu_http_option_t* option);

/* ctrl option
 *
 * @param option        the option
 * @param ctrl          the ctrl code
 * @param args          the ctrl args
 */
xu_bool_t xu_http_option_ctrl(xu_http_option_t* option, xu_size_t code, xu_va_list_t args);

#ifdef __xu_debug__
/* dump option
 *
 * @param option        the option
 */
xu_void_t xu_http_option_dump(xu_http_option_t* option);
#endif
