#include "xutil/libm/math.h"
#include "xutil/platform/time.h"
#include "xutil/utils/used.h"

/* *******************************************************
 * private implementation
 */
static xu_uint32_t xu_isqrti64_impl(xu_uint64_t x)
{
    // compute it fast for uint32
    if (!(x >> 32)) return xu_isqrti((xu_uint32_t)x);
    // compute it for uint64
    else
    {
        // done
        xu_uint64_t t;
        xu_uint64_t n = 0;
        xu_uint64_t b = 0x80000000;
        xu_uint32_t s = 31;
        do
        {
            if (x >= (t = (((n << 1) + b) << s--)))
            {
                n += b;
                x -= t;
            }
        } while (b >>= 1);

        // check
        xu_assert(!(n >> 32));

        // ok
        return (xu_uint32_t)n;
    }
}
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
static __xu_inline__ xu_uint32_t xu_isqrti64_impl_using_sqrt(xu_uint64_t x)
{
    return (!(x >> 32)) ? (xu_uint32_t)xu_sqrtf((xu_float_t)x) : (xu_uint32_t)xu_sqrt((xu_double_t)x);
}
#endif

/* *******************************************************
 * implementation
 */
xu_uint32_t xu_isqrti64(xu_uint64_t x)
{
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT

    // using the sqrt function?
    static xu_long_t s_using_sqrt = -1;

    // analyze the profile
    if (s_using_sqrt < 0)
    {
        // analyze isqrti64
        xu_hong_t                   t1 = xu_uclock();
        __xu_volatile__ xu_size_t   n1 = 100;
        __xu_volatile__ xu_uint32_t v1;
        xu_used(&v1);
        while (n1--)
        {
            v1 = xu_isqrti64_impl((1 << 4) + 3);
            v1 = xu_isqrti64_impl((1 << 12) + 3);
            v1 = xu_isqrti64_impl((1 << 20) + 3);
            v1 = xu_isqrti64_impl((1 << 28) + 3);
            v1 = xu_isqrti64_impl((1ULL << 36) + 3);
            v1 = xu_isqrti64_impl((1ULL << 42) + 3);
            v1 = xu_isqrti64_impl((1ULL << 50) + 3);
            v1 = xu_isqrti64_impl((1ULL << 58) + 3);
            v1 = xu_isqrti64_impl((1ULL << 60) + 3);
        }
        t1 = xu_uclock() - t1;

        // analyze sqrt
        xu_hong_t                   t2 = xu_uclock();
        __xu_volatile__ xu_size_t   n2 = 100;
        __xu_volatile__ xu_uint32_t v2;
        xu_used(&v2);
        while (n2--)
        {
            v2 = xu_isqrti64_impl_using_sqrt((1 << 4) + 3);
            v2 = xu_isqrti64_impl_using_sqrt((1 << 12) + 3);
            v2 = xu_isqrti64_impl_using_sqrt((1 << 20) + 3);
            v2 = xu_isqrti64_impl_using_sqrt((1 << 28) + 3);
            v2 = xu_isqrti64_impl_using_sqrt((1ULL << 36) + 3);
            v2 = xu_isqrti64_impl_using_sqrt((1ULL << 42) + 3);
            v2 = xu_isqrti64_impl_using_sqrt((1ULL << 50) + 3);
            v2 = xu_isqrti64_impl_using_sqrt((1ULL << 58) + 3);
            v2 = xu_isqrti64_impl_using_sqrt((1ULL << 60) + 3);
        }
        t2 = xu_uclock() - t2;

        // using sqrt?
        s_using_sqrt = t2 < t1 ? 1 : 0;
    }

    // done
    return s_using_sqrt > 0 ? xu_isqrti64_impl_using_sqrt(x) : xu_isqrti64_impl(x);
#else
    return xu_isqrti64_impl(x);
#endif
}
