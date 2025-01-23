#pragma once

#include "xutil/network/ipaddr.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 */

// the dns looker type
typedef __xu_typeref__(dns_looker);

/* *******************************************************
 * interfaces
 */

/*! init for looking ipv4 from the host name, non-block
 *
 * @param name      the host name
 *
 * @return          the looker handle
 */
xu_dns_looker_ref_t xu_dns_looker_init(xu_char_t const* name);

/*! spak the looker
 *
 * @param looker    the looker
 * @param addr      the address
 *
 * @return          1: ok, 0: continue: -1: failed
 */
xu_long_t xu_dns_looker_spak(xu_dns_looker_ref_t looker, xu_ipaddr_ref_t addr);

/*! wait the looker
 *
 * @param looker    the looker
 * @param timeout   the timeout
 *
 * @return          1: ok, 0: continue: -1: failed
 */
xu_long_t xu_dns_looker_wait(xu_dns_looker_ref_t looker, xu_long_t timeout);

/*! exit the looker
 *
 * @param looker    the looker
 */
xu_void_t xu_dns_looker_exit(xu_dns_looker_ref_t looker);

/*! lookup address from the host name, block
 *
 * try to look it from cache first
 *
 * @param name      the host name
 * @param addr      the address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_dns_looker_done(xu_char_t const* name, xu_ipaddr_ref_t addr);
