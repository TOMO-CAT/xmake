#include "xutil/network/dns/server.h"
#include "xutil/platform/dynamic.h"
#include "xutil/prefix.h"
#include "xutil/utils/trace.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>

/* *******************************************************
 *  types
 * *******************************************************
 */

// the res_ninit func type
typedef xu_int_t (*xu_dns_res_ninit_func_t)(res_state);

/* *******************************************************
 *  interfaces
 * *******************************************************
 */
xu_bool_t xu_dns_init_env()
{
    // done
    xu_size_t        count   = 0;
    xu_dynamic_ref_t library = xu_dynamic_init("libresolv.dylib");
    if (library)
    {
        // the res_ninit func
        xu_dns_res_ninit_func_t pres_ninit = (xu_dns_res_ninit_func_t)xu_dynamic_func(library, "res_9_ninit");
        if (pres_ninit)
        {
            // init state
            struct __res_state state;
            if (!pres_ninit(&state))
            {
                // walk it
                xu_size_t i = 0;
                for (i = 0; i < state.nscount; i++, count++)
                {
                    // the address
                    xu_char_t const* addr = inet_ntoa(state.nsaddr_list[i].sin_addr);
                    xu_assert_and_check_continue(addr);

                    // trace
                    xu_trace_d("addr: %s", addr);

                    // add address
                    xu_dns_server_add(addr);
                }
            }
        }
    }

    // ok
    return xu_true;
}
xu_void_t xu_dns_exit_env() {}
