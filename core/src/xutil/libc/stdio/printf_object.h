#pragma once

#include "xutil/algorithm/binary_find_if.h"
#include "xutil/container/array_iterator.h"
#include "xutil/container/iterator.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  macros
 */

// the printf object name maxn
#define XU_PRINTF_OBJECT_NAME_MAXN (16)

/* *******************************************************
 *  types
 */

/*! the printf object func type
 *
 * @param object        the printf object
 * @param cstr          the string buffer
 * @param maxn          the string buffer maxn
 *
 * @return              the real string size
 */
typedef xu_long_t (*xu_printf_object_func_t)(xu_cpointer_t object, xu_char_t* cstr, xu_size_t maxn);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! register the printf object func
 *
 * @note non thread-safe
 *
 * @param name          the format name
 * @param func          the format func
 *
 * @code
    static xu_long_t xu_printf_object_ipv4(xu_cpointer_t object, xu_char_t* cstr, xu_size_t maxn)
    {
        // check
        xu_assert_and_check_return_val(object && cstr && maxn, -1);

        // the ipv4
        xu_ipv4_ref_t ipv4 = (xu_ipv4_ref_t)object;

        // format
        xu_long_t size = xu_snprintf(cstr, maxn - 1, "%u.%u.%u.%u", ipv4->u8[0], ipv4->u8[1], ipv4->u8[2], ipv4->u8[3]);
        if (size >= 0) cstr[size] = '\0';

        // ok?
        return size;
    }

    // register the "ipv4" printf object func
    xu_printf_object_register("ipv4", xu_printf_object_ipv4);

    // init ipv4
    xu_ipv4_t ipv4;
    xu_ipv4_cstr_set(&ipv4, "127.0.0.1");

    // trace ipv4, output: "ipv4: 127.0.0.1"
    xu_trace_i("ipv4: %{ipv4}", &ipv4);

 * @endcode
 */
xu_void_t xu_printf_object_register(xu_char_t const* name, xu_printf_object_func_t func);

/*! find the printf object func from the given format name
 *
 * @param name          the format name
 *
 * @return              the format func
 */
xu_printf_object_func_t xu_printf_object_find(xu_char_t const* name);

/*! exit the printf object
 */
xu_void_t xu_printf_object_exit(xu_noarg_t);

#ifdef __xu_debug__
/*! dump the printf object
 */
xu_void_t xu_printf_object_dump(xu_noarg_t);
#endif
