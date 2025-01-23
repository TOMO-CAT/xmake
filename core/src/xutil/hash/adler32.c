#include "xutil/hash/adler32.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"

#ifdef XU_CONFIG_PACKAGE_HAVE_ZLIB
#    include <zlib.h>
#endif

/* *******************************************************
 * macros
 */

// largest prime smaller than 65536
#define BASE (65521)

// NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1
#define NMAX (5552)

// dot
#define DO1(data, i)                                                                                                   \
    {                                                                                                                  \
        adler += (data)[i];                                                                                            \
        sum2 += adler;                                                                                                 \
    }
#define DO2(data, i)                                                                                                   \
    DO1(data, i);                                                                                                      \
    DO1(data, i + 1);
#define DO4(data, i)                                                                                                   \
    DO2(data, i);                                                                                                      \
    DO2(data, i + 2);
#define DO8(data, i)                                                                                                   \
    DO4(data, i);                                                                                                      \
    DO4(data, i + 4);
#define DO16(data)                                                                                                     \
    DO8(data, 0);                                                                                                      \
    DO8(data, 8);

// mod
#define MOD(a) (a) %= BASE
#define MOD28(a) (a) %= BASE
#define MOD63(a) (a) %= BASE

/* *******************************************************
 * implementation
 */

xu_uint32_t xu_adler32_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
#ifdef XU_CONFIG_PACKAGE_HAVE_ZLIB
    return adler32(seed, data, (xu_uint_t)size);
#else
    // split adler-32 into component sums
    xu_uint32_t adler = seed;
    xu_size_t   sum2  = (adler >> 16) & 0xffff;
    adler &= 0xffff;

    // in case user likes doing a byte at a time, keep it fast
    if (size == 1)
    {
        adler += data[0];
        if (adler >= BASE) adler -= BASE;

        sum2 += adler;
        if (sum2 >= BASE) sum2 -= BASE;

        // ok?
        return (xu_uint32_t)(adler | (sum2 << 16));
    }

    // initial adler-32 value (deferred check for size == 1 speed)
    xu_check_return_val(data, 1);

    // in case short lengths are provided, keep it somewhat fast
    if (size < 16)
    {
        // done
        while (size--)
        {
            adler += *data++;
            sum2 += adler;
        }
        if (adler >= BASE) adler -= BASE;

        // only added so many BASE's
        MOD28(sum2);

        // ok?
        return (xu_uint32_t)(adler | (sum2 << 16));
    }

    // do length NMAX blocks -- requires just one modulo operation
    xu_size_t n;
    while (size >= NMAX)
    {
        size -= NMAX;

        // NMAX is divisible by 16
        n = NMAX / 16;
        do
        {
            // 16 sums unrolled
            DO16(data);
            data += 16;

        } while (--n);

        MOD(adler);
        MOD(sum2);
    }

    // do remaining bytes (less than NMAX, still just one modulo)
    if (size)
    {
        // avoid modulos if none remaining
        while (size >= 16)
        {
            size -= 16;
            DO16(data);
            data += 16;
        }
        while (size--)
        {
            adler += *data++;
            sum2 += adler;
        }
        MOD(adler);
        MOD(sum2);
    }

    // return recombined sums
    return (xu_uint32_t)(adler | (sum2 << 16));
#endif
}
xu_uint32_t xu_adler32_make_from_cstr(xu_char_t const* cstr, xu_uint32_t seed)
{
    // check
    xu_assert_and_check_return_val(cstr, 0);

    // make it
    return xu_adler32_make((xu_byte_t const*)cstr, xu_strlen(cstr) + 1, seed);
}
