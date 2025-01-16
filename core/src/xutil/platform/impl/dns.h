#pragma once

#include "xutil/network/dns/server.h"
#include "xutil/prefix.h"

/* init dns environment
 *
 * @return  xu_true or xu_false
 */
xu_bool_t xu_dns_init_env(xu_noarg_t);

// exit dns environment
xu_void_t xu_dns_exit_env(xu_noarg_t);
