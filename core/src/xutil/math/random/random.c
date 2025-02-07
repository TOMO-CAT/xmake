#include "xutil/math/random/random.h"
#include "xutil/macros/assert.h"
#include "xutil/platform/file.h"
#include "xutil/platform/time.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * macros
 */

// the initial seed
#define XU_RANDOM_SEED_INIT (2166136261ul)

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_LIBC_HAVE_RANDOM) && defined(XU_CONFIG_LIBC_HAVE_SRANDOM)
#    include "xutil/platform/libc/random.c"
#else
xu_void_t xu_random_seed(xu_size_t seed) { xu_random_linear_seed(seed); }
xu_long_t xu_random_value() { return xu_random_linear_value(); }
#endif
xu_void_t xu_random_reset(xu_bool_t pseudo)
{
    // init seed
    xu_size_t seed = XU_RANDOM_SEED_INIT;
    if (!pseudo)
    {
        // init read
        xu_size_t read = 0;

#ifndef XU_CONFIG_OS_WINDOWS
        // attempt to read seed from /dev/urandom
        xu_file_ref_t file = xu_file_init("/dev/urandom", XU_FILE_MODE_RO);
        if (file)
        {
            // read seed
            xu_byte_t* data = (xu_byte_t*)&seed;
            while (read < sizeof(xu_size_t))
            {
                // read it
                xu_long_t real = xu_file_read(file, data + read, sizeof(xu_size_t) - read);
                xu_assert_and_check_break(real > 0);

                // update size
                read += real;
            }

            // exit file
            xu_file_exit(file);
        }
#endif

        // init seed using clock if read failed?
        if (read != sizeof(xu_size_t))
        {
            // get clock
            xu_uint64_t clock = (xu_uint64_t)xu_uclock();

            // init seed using clock
            seed = (xu_size_t)((clock >> 32) ^ clock);

            // xor the stack address
            seed ^= (xu_size_t)xu_p2u32(&seed);
        }
    }

    // reset seed
    xu_random_seed(seed);
}
xu_long_t xu_random_range(xu_long_t begin, xu_long_t end)
{
    // check
    xu_assert_and_check_return_val(begin < end, begin);

    // make range
    return (begin + (xu_long_t)((xu_size_t)xu_random_value() % (end - begin)));
}
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
xu_float_t xu_random_rangef(xu_float_t begin, xu_float_t end)
{
    // check
    xu_assert_and_check_return_val(begin < end, begin);

    // the factor
    xu_double_t factor = (xu_double_t)xu_random_range(0, XU_MAXS32) / (xu_double_t)XU_MAXS32;

    // the value
    return (xu_float_t)((end - begin) * factor);
}
#endif
