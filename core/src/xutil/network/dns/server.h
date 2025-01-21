#pragma once

#include "xutil/network/ipaddr.h"
#include "xutil/prefix.h"

/*! init the server list
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_dns_server_init(xu_noarg_t);

// exit the server list
xu_void_t xu_dns_server_exit(xu_noarg_t);

// dump the server list
xu_void_t xu_dns_server_dump(xu_noarg_t);

// sort the server list by the response speed
xu_void_t xu_dns_server_sort(xu_noarg_t);

/*! get the server
 *
 * @param addr      the server address list, addr[0] is the fastest
 *
 * @return          the server size
 */
xu_size_t xu_dns_server_get(xu_ipaddr_t addr[2]);

/*! add the server
 *
 * @param addr      the server address
 */
xu_void_t xu_dns_server_add(xu_char_t const* addr);
