#pragma once

#include "xutil/network/ipaddr.h"
#include "xutil/prefix.h"

/* *******************************************************
 * interfaces
 */

/*! init the cache list
 *
 * not using ctime default
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_dns_cache_init(xu_noarg_t);

/// exit the cache list
xu_void_t xu_dns_cache_exit(xu_noarg_t);

/*! get addr from cache
 *
 * @param name      the host name
 * @param addr      the host addr
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_dns_cache_get(xu_char_t const* name, xu_ipaddr_ref_t addr);

/*! set addr to cache
 *
 * @param name      the host name
 * @param addr      the host addr
 */
xu_void_t xu_dns_cache_set(xu_char_t const* name, xu_ipaddr_ref_t addr);
