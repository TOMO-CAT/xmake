#include "xutil/math/impl/math.h"

/* *******************************************************
 * private implementation
 * *******************************************************
 */

#ifndef XU_CONFIG_MICRO_ENABLE
static xu_long_t xu_math_printf_format_fixed(xu_cpointer_t object, xu_char_t* cstr, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(cstr && maxn, -1);

    // the fixed
    xu_fixed_t fixed = (xu_fixed_t)xu_p2s32(object);

    // format
#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
    xu_long_t size = xu_snprintf(cstr, maxn - 1, "%f", xu_fixed_to_float(fixed));
    if (size >= 0) cstr[size] = '\0';
#    else
    xu_long_t size = xu_snprintf(cstr, maxn - 1, "%ld", xu_fixed_to_long(fixed));
    if (size >= 0) cstr[size] = '\0';
#    endif

    // ok?
    return size;
}
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_bool_t xu_math_init_env()
{
#ifndef XU_CONFIG_MICRO_ENABLE
    // register printf("%{fixed}", fixed);
    xu_printf_object_register("fixed", xu_math_printf_format_fixed);
#endif

    // ok
    return xu_true;
}
xu_void_t xu_math_exit_env() {}
