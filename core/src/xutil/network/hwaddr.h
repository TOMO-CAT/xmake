#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

// the hwaddr string data maxn
#define XU_HWADDR_CSTR_MAXN (18)

/* *******************************************************
 * types
 */
/*! the hardware address type
 *
 * mac: xx:xx:xx:xx:xx:xx
 */
typedef struct __xu_hwaddr_t
{
    // u8
    xu_byte_t u8[6];

} xu_hwaddr_t, *xu_hwaddr_ref_t;

/* *******************************************************
 * interfaces
 */

/*! clear the hwaddr
 *
 * @param hwaddr    the hwaddr
 */
xu_void_t xu_hwaddr_clear(xu_hwaddr_ref_t hwaddr);

/*! copy address, faster than *hwaddr = *other
 *
 * @param hwaddr    the address
 * @param copied    the copied address
 */
xu_void_t xu_hwaddr_copy(xu_hwaddr_ref_t hwaddr, xu_hwaddr_ref_t copied);

/*! is equal?
 *
 * @param hwaddr    the hwaddr
 * @param other     the other hwaddr
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_hwaddr_is_equal(xu_hwaddr_ref_t hwaddr, xu_hwaddr_ref_t other);

/*! get the hwaddr string
 *
 * @param hwaddr    the hwaddr
 * @param data      the hwaddr data
 * @param maxn      the data maxn
 *
 * @return          the hwaddr address
 */
xu_char_t const* xu_hwaddr_cstr(xu_hwaddr_ref_t hwaddr, xu_char_t* data, xu_size_t maxn);

/*! set the hwaddr from string
 *
 * @param hwaddr    the hwaddr
 * @param cstr      the hwaddr string
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_hwaddr_cstr_set(xu_hwaddr_ref_t hwaddr, xu_char_t const* cstr);
