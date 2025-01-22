#pragma once

#include "xutil/network/ipaddr.h"
#include "xutil/prefix.h"
#include "xutil/string/string.h"

/* *******************************************************
 *  types
 */

// the url protocol type
typedef enum __xu_url_protocol_t
{
    XU_URL_PROTOCOL_NONE = 0,
    XU_URL_PROTOCOL_FILE = 1 //!< file://...
    ,
    XU_URL_PROTOCOL_SOCK = 2 //!< sock://... or socks://...
    ,
    XU_URL_PROTOCOL_HTTP = 3 //!< http://... or https://...
    ,
    XU_URL_PROTOCOL_DATA = 4 //!< data://...
    ,
    XU_URL_PROTOCOL_RTSP = 5 //!< rtsp://...
    ,
    XU_URL_PROTOCOL_SQL = 6 //!< sql://...

} xu_url_protocol_t;

// the url type
typedef struct __xu_url_t
{
    // the protocol
    xu_uint16_t protocol : 6;

    // is ssl?
    xu_uint16_t is_ssl : 1;

    // the addr
    xu_ipaddr_t addr;

    // the host
    xu_string_t host;

    // the path
    xu_string_t path;

    // the args
    xu_string_t args;

    // the url cache
    xu_string_t cache;

} xu_url_t, *xu_url_ref_t;

/* *******************************************************
 *  interfaces
 */
/*! init url
 *
 * @param url       the url
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_url_init(xu_url_ref_t url);

/*! init url from c-string
 *
 * @param url       the url
 * @param cstr      the url c-string
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_url_init_from_cstr(xu_url_ref_t url, xu_char_t const* cstr);

/*! exit url
 *
 * @param url       the url
 */
xu_void_t xu_url_exit(xu_url_ref_t url);

/*! clear url
 *
 * @param url       the url
 */
xu_void_t xu_url_clear(xu_url_ref_t url);

/*! get the url c-string
 *
 * @param url       the url
 *
 * @return          the c-string
 */
xu_char_t const* xu_url_cstr(xu_url_ref_t url);

/*! set the url c-string
 *
 * @param url       the url
 * @param cstr      the url c-string
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_url_cstr_set(xu_url_ref_t url, xu_char_t const* cstr);

/*! copy the url
 *
 * @param url       the url
 * @param copy      the copied url
 */
xu_void_t xu_url_copy(xu_url_ref_t url, xu_url_ref_t copy);

/*! is ssl?
 *
 * @param url       the url
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_url_ssl(xu_url_ref_t url);

/*! set ssl
 *
 * @param url       the url
 * @param is_ssl    is ssl?
 */
xu_void_t xu_url_ssl_set(xu_url_ref_t url, xu_bool_t is_ssl);

/*! get the protocol from the url
 *
 * @param url       the url
 *
 * @return          the url protocol
 */
xu_size_t xu_url_protocol(xu_url_ref_t url);

/*! set the protocol to the url
 *
 * @param url       the url
 * @param protocol  the url protocol
 */
xu_void_t xu_url_protocol_set(xu_url_ref_t url, xu_size_t protocol);

/*! the protocol c-string
 *
 * @param url       the url
 *
 * @return          the url protocol c-string
 */
xu_char_t const* xu_url_protocol_cstr(xu_url_ref_t url);

/*! probe the protocol from the url
 *
 * @param url       the url
 *
 * @return          the url protocol
 */
xu_size_t xu_url_protocol_probe(xu_char_t const* url);

/*! get the port from the url
 *
 * @param url       the url
 *
 * @return          the url port
 */
xu_uint16_t xu_url_port(xu_url_ref_t url);

/*! set the port to the url
 *
 * @param url       the url
 * @param port      the url port
 */
xu_void_t xu_url_port_set(xu_url_ref_t url, xu_uint16_t port);

/*! get the address from the url
 *
 * @param url       the url
 *
 * @return          the url address
 */
xu_ipaddr_ref_t xu_url_addr(xu_url_ref_t url);

/*! set the address to the url
 *
 * @param url       the url
 * @param addr      the url address
 */
xu_void_t xu_url_addr_set(xu_url_ref_t url, xu_ipaddr_ref_t addr);

/*! get the host from the url
 *
 * @param url       the url
 *
 * @return          the url host
 */
xu_char_t const* xu_url_host(xu_url_ref_t url);

/*! set the host to the url
 *
 * @param url       the url
 * @param host      the url host
 */
xu_void_t xu_url_host_set(xu_url_ref_t url, xu_char_t const* host);

/*! get the path from the url
 *
 * @param url       the url
 *
 * @return          the url path
 */
xu_char_t const* xu_url_path(xu_url_ref_t url);

/*! set the path to the url
 *
 * @param url       the url
 * @param path      the url path
 */
xu_void_t xu_url_path_set(xu_url_ref_t url, xu_char_t const* path);

/*! get the arguments from the url
 *
 * @param url       the url
 *
 * @return          the url arguments
 */
xu_char_t const* xu_url_args(xu_url_ref_t url);

/*! set the arguments to the url
 *
 * @param url       the url
 * @param args      the url arguments
 */
xu_void_t xu_url_args_set(xu_url_ref_t url, xu_char_t const* args);
