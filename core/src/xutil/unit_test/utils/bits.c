#include "xutil/unit_test/ut.h"

/* *******************************************************
 * test
 */
static xu_void_t xu_test_bits_swap_u16()
{
    __xu_volatile__ xu_uint16_t x  = 0x1234;
    __xu_volatile__ xu_size_t   n  = 100000001;
    __xu_volatile__ xu_hong_t   dt = xu_mclock();

    while (n--)
        x = xu_bits_swap_u16(x);

    dt = xu_mclock() - dt;
    xu_trace_i("[bitops]: swap_u16: %x => %x, %llu ms", 0x1234, x, dt);
}
static xu_void_t xu_test_bits_swap_u32()
{
    __xu_volatile__ xu_uint32_t x  = 0x12345678;
    __xu_volatile__ xu_size_t   n  = 100000001;
    __xu_volatile__ xu_hong_t   dt = xu_mclock();

    while (n--)
        x = xu_bits_swap_u32(x);

    dt = xu_mclock() - dt;
    xu_trace_i("[bitops]: swap_u32: %x => %x, %llu ms", 0x12345678, x, dt);
}
static xu_void_t xu_test_bits_swap_u64()
{
    __xu_volatile__ xu_hize_t x  = 0x12345678;
    __xu_volatile__ xu_size_t n  = 100000001;
    __xu_volatile__ xu_hong_t dt = xu_mclock();

    while (n--)
        x = xu_bits_swap_u64(x);

    dt = xu_mclock() - dt;
    xu_trace_i("[bitops]: swap_u64: %x => %llx, %llu ms", 0x12345678, x, dt);
}
static xu_void_t xu_test_bits_ubits32(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t b  = 0;
    __xu_volatile__ xu_size_t n  = 0;
    __xu_volatile__ xu_hong_t ts = 0;
    __xu_volatile__ xu_hong_t tg = 0;
    for (b = 0; b < 8; ++b)
    {
        xu_trace_i("[bitops]: ubits32 b: %d x: %x", b, x);
        for (n = 0; n <= 32; ++n)
        {
            xu_byte_t                   p[256] = {0};
            __xu_volatile__ xu_size_t   n1     = 100000;
            __xu_volatile__ xu_size_t   n2     = 100000;
            __xu_volatile__ xu_uint32_t y      = 0;
            __xu_volatile__ xu_hong_t   t1, t2;
            __xu_volatile__ xu_uint32_t xx = n < 32 ? (x & ((xu_uint32_t)(1 << n) - 1)) : x;

            t1 = xu_mclock();
            while (n1--)
                xu_bits_set_ubits32(p, b, x, n);
            t1 = xu_mclock() - t1;
            ts += t1;

            t2 = xu_mclock();
            while (n2--)
                y = xu_bits_get_ubits32(p, b, n);
            t2 = xu_mclock() - t2;
            tg += t1;

            if (xx != y)
            {
                xu_trace_i("[bitops]: ubits32 set: b: %x, n: %x, x: %x, %llu ms", b, n, xx, t1);
                xu_trace_i("[bitops]: ubits32 get: b: %x, n: %x, y: %x, %llu ms", b, n, y, t2);
            }
        }
    }
    xu_trace_i("[bitops]: ubits32 set: %llu ms, get: %llu ms", ts, tg);
}
static xu_void_t xu_test_bits_sbits32(xu_sint32_t x)
{
    __xu_volatile__ xu_size_t b  = 0;
    __xu_volatile__ xu_size_t n  = 0;
    __xu_volatile__ xu_hong_t ts = 0;
    __xu_volatile__ xu_hong_t tg = 0;
    for (b = 0; b < 8; ++b)
    {
        xu_trace_i("[bitops]: sbits32 b: %d x: %d", b, x);
        for (n = 2; n <= 32; ++n)
        {
            xu_byte_t                   p[256] = {0};
            __xu_volatile__ xu_size_t   n1     = 100000;
            __xu_volatile__ xu_size_t   n2     = 100000;
            __xu_volatile__ xu_sint32_t y      = 0;
            __xu_volatile__ xu_hong_t   t1, t2;
            __xu_volatile__ xu_sint32_t xx = ((x >> 31) << (n - 1)) | (x & ((xu_uint32_t)(1 << (n - 1)) - 1));

            t1 = xu_mclock();
            while (n1--)
                xu_bits_set_sbits32(p, b, x, n);
            t1 = xu_mclock() - t1;
            ts += t1;

            t2 = xu_mclock();
            while (n2--)
                y = xu_bits_get_sbits32(p, b, n);
            t2 = xu_mclock() - t2;
            tg += t1;

            if (xx != y)
            {
                xu_trace_i("[bitops]: sbits32 set: b: %d, n: %u, x: %d, %llu ms", b, n, xx, t1);
                xu_trace_i("[bitops]: sbits32 get: b: %u, n: %u, y: %d, %llu ms", b, n, y, t2);
            }
        }
    }
    xu_trace_i("[bitops]: sbits32 set: %llu ms, get: %llu ms", ts, tg);
}
static xu_void_t xu_test_bits_u32_be(xu_uint32_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_uint32_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_u32_be(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_u32_be(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: u32_be set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_u32_le(xu_uint32_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_uint32_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_u32_le(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_u32_le(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: u32_le set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_u24_be(xu_uint32_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_uint32_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_u24_be(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_u24_be(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: u24_be set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_u24_le(xu_uint32_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_uint32_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_u24_le(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_u24_le(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: u24_le set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_u16_be(xu_uint16_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_uint16_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_u16_be(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_u16_be(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: u16_be set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_u16_le(xu_uint16_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_uint16_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_u16_le(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_u16_le(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: u16_le set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}

static xu_void_t xu_test_bits_s32_be(xu_sint32_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_sint32_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_s32_be(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_s32_be(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: s32_be set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_s32_le(xu_sint32_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_sint32_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_s32_le(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_s32_le(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: s32_le set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_s24_be(xu_sint32_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_sint32_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_s24_be(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_s24_be(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: s24_be set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_s24_le(xu_sint32_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_sint32_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_s24_le(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_s24_le(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: s24_le set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_s16_be(xu_sint16_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_sint16_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_s16_be(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_s16_be(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: s16_be set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_s16_le(xu_sint16_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_sint16_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_s16_le(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_s16_le(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: s16_le set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
static xu_void_t xu_test_bits_double_bbe(xu_double_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_double_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_double_bbe(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_double_bbe(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: double_bbe set: x: %lf, %llu ms, get: y: %lf, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_double_ble(xu_double_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_double_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_double_ble(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_double_ble(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: double_ble set: x: %lf, %llu ms, get: y: %lf, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_double_lbe(xu_double_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_double_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_double_lbe(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_double_lbe(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: double_lbe set: x: %lf, %llu ms, get: y: %lf, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_double_lle(xu_double_t x)
{
    xu_byte_t                   p[8] = {0};
    __xu_volatile__ xu_double_t y    = 0;
    __xu_volatile__ xu_size_t   n1   = 100000;
    __xu_volatile__ xu_size_t   n2   = 100000;
    __xu_volatile__ xu_hong_t   ts   = 0;
    __xu_volatile__ xu_hong_t   tg   = 0;
    __xu_volatile__ xu_hong_t   t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_double_lle(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_double_lle(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: double_lle set: x: %lf, %llu ms, get: y: %lf, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_float_be(xu_float_t x)
{
    xu_byte_t                  p[8] = {0};
    __xu_volatile__ xu_float_t y    = 0;
    __xu_volatile__ xu_size_t  n1   = 100000;
    __xu_volatile__ xu_size_t  n2   = 100000;
    __xu_volatile__ xu_hong_t  ts   = 0;
    __xu_volatile__ xu_hong_t  tg   = 0;
    __xu_volatile__ xu_hong_t  t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_float_be(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_float_be(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: float_be set: x: %f, %llu ms, get: y: %f, %llu ms", x, ts, y, tg);
}
static xu_void_t xu_test_bits_float_le(xu_float_t x)
{
    xu_byte_t                  p[8] = {0};
    __xu_volatile__ xu_float_t y    = 0;
    __xu_volatile__ xu_size_t  n1   = 100000;
    __xu_volatile__ xu_size_t  n2   = 100000;
    __xu_volatile__ xu_hong_t  ts   = 0;
    __xu_volatile__ xu_hong_t  tg   = 0;
    __xu_volatile__ xu_hong_t  t1, t2;

    t1 = xu_mclock();
    while (n1--)
        xu_bits_set_float_le(p, x);
    t1 = xu_mclock() - t1;
    ts += t1;

    t2 = xu_mclock();
    while (n2--)
        y = xu_bits_get_float_le(p);
    t2 = xu_mclock() - t2;
    tg += t1;

    xu_trace_i("[bitops]: float_le set: x: %f, %llu ms, get: y: %f, %llu ms", x, ts, y, tg);
}
#endif
static xu_void_t xu_test_bits_cl0_u32_be(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cl0_u32_be(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32_be: cl0: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cl0_u32_le(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cl0_u32_le(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32_le: cl0: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cl0_u64_be(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cl0_u64_be(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64_be: cl0: %064llb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cl0_u64_le(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cl0_u64_le(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64_le: cl0: %064llb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cl1_u32_be(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cl1_u32_be(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32_be: cl1: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cl1_u32_le(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cl1_u32_le(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32_le: cl1: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cl1_u64_be(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cl1_u64_be(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64_be: cl1: %064llb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cl1_u64_le(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cl1_u64_le(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64_le: cl1: %064llb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cb0_u32(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cb0_u32(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32: cb0: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cb0_u64(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cb0_u64(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64: cb0: %064llb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cb1_u32(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cb1_u32(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32: cb1: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_cb1_u64(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_cb1_u64(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64: cb1: %064llb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_fb0_u32_be(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_fb0_u32_be(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32_be: fb0: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_fb0_u32_le(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_fb0_u32_le(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32_le: fb0: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_fb0_u64_be(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_fb0_u64_be(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64_be: fb0: %064llb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_fb0_u64_le(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_fb0_u64_le(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64_le: fb0: %064llb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_fb1_u32_be(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_fb1_u32_be(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32_be: fb1: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_fb1_u32_le(xu_uint32_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_fb1_u32_le(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u32_le: fb1: %032lb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_fb1_u64_be(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_fb1_u64_be(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64_be: fb1: %064llb => %lu, %lld ms", x, i, t);
}
static xu_void_t xu_test_bits_fb1_u64_le(xu_uint64_t x)
{
    __xu_volatile__ xu_size_t i = 0;
    __xu_volatile__ xu_size_t n = 100000;
    __xu_volatile__ xu_hong_t t = xu_mclock();
    while (n--)
        i = xu_bits_fb1_u64_le(x);
    t = xu_mclock() - t;

    xu_trace_i("[bitops]: u64_le: fb1: %064llb => %lu, %lld ms", x, i, t);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_utils_bits_main(xu_int_t argc, xu_char_t** argv)
{
#if 1
    xu_test_bits_swap_u16();
    xu_test_bits_swap_u32();
    xu_test_bits_swap_u64();
#endif

#if 1
    xu_trace_i("");
    xu_test_bits_ubits32(0x87654321);
    xu_test_bits_ubits32(0x12345678);

    xu_trace_i("");
    xu_test_bits_sbits32(0x87654321);
    xu_test_bits_sbits32(0x12345678);

    xu_trace_i("");
    xu_test_bits_sbits32(-300);
    xu_test_bits_sbits32(300);

    xu_trace_i("");
    xu_test_bits_u32_be(0x87654321);
    xu_test_bits_u32_be(0x12345678);

    xu_test_bits_u32_le(0x87654321);
    xu_test_bits_u32_le(0x12345678);

    xu_trace_i("");
    xu_test_bits_u24_be(0x654321);
    xu_test_bits_u24_be(0x345678);

    xu_test_bits_u24_le(0x654321);
    xu_test_bits_u24_le(0x345678);

    xu_trace_i("");
    xu_test_bits_u16_be(0x4321);
    xu_test_bits_u16_be(0x5678);

    xu_test_bits_u16_le(0x4321);
    xu_test_bits_u16_le(0x5678);

    xu_trace_i("");
    xu_test_bits_s32_be(0x8765F321);
    xu_test_bits_s32_be(0x1234F678);

    xu_test_bits_s32_le(0x8765F321);
    xu_test_bits_s32_le(0x1234F678);

    xu_trace_i("");
    xu_test_bits_s24_be(123456);
    xu_test_bits_s24_be(-123456);

    xu_test_bits_s24_le(123456);
    xu_test_bits_s24_le(-123456);

    xu_trace_i("");
    xu_test_bits_s16_be(4321);
    xu_test_bits_s16_be(-4321);

    xu_test_bits_s16_le(4321);
    xu_test_bits_s16_le(-4321);

#    ifdef XU_CONFIG_TYPE_HAVE_FLOAT
    xu_trace_i("");
    xu_test_bits_double_bbe(3.1415926);
    xu_test_bits_double_ble(3.1415926);

    xu_test_bits_double_lbe(3.1415926);
    xu_test_bits_double_lle(3.1415926);

    xu_trace_i("");
    xu_test_bits_float_be(3.1415926f);
    xu_test_bits_float_le(3.1415926f);

    xu_test_bits_float_be(3.1415926f);
    xu_test_bits_float_le(3.1415926f);
#    endif
#endif

#if 1
    xu_trace_i("");
    xu_test_bits_cl0_u32_be(0);
    xu_test_bits_cl0_u32_be(1);
    xu_test_bits_cl0_u32_be(2);
    xu_test_bits_cl0_u32_be(8);
    xu_test_bits_cl0_u32_be(123);
    xu_test_bits_cl0_u32_be(123456);
    xu_test_bits_cl0_u32_be(800000);
    xu_test_bits_cl0_u32_be(31415926);
    xu_test_bits_cl0_u32_be(0xffffffff);
    xu_test_bits_cl0_u32_be(0xfffffff0);
    xu_test_bits_cl0_u32_be(0x0fffffff);
    xu_test_bits_cl0_u32_be(0xfff00fff);
    xu_test_bits_cl0_u32_be(0x0ffffff0);
    xu_test_bits_cl0_u32_be(0x0000000f);
    xu_test_bits_cl0_u32_be(0xf0000000);

    xu_trace_i("");
    xu_test_bits_cl0_u32_le(0);
    xu_test_bits_cl0_u32_le(1);
    xu_test_bits_cl0_u32_le(2);
    xu_test_bits_cl0_u32_le(8);
    xu_test_bits_cl0_u32_le(123);
    xu_test_bits_cl0_u32_le(123456);
    xu_test_bits_cl0_u32_le(800000);
    xu_test_bits_cl0_u32_le(31415926);
    xu_test_bits_cl0_u32_le(0xffffffff);
    xu_test_bits_cl0_u32_le(0xfffffff0);
    xu_test_bits_cl0_u32_le(0x0fffffff);
    xu_test_bits_cl0_u32_le(0xfff00fff);
    xu_test_bits_cl0_u32_le(0x0ffffff0);
    xu_test_bits_cl0_u32_le(0x0000000f);
    xu_test_bits_cl0_u32_le(0xf0000000);

    xu_trace_i("");
    xu_test_bits_cl0_u64_be(0);
    xu_test_bits_cl0_u64_be(1);
    xu_test_bits_cl0_u64_be(2);
    xu_test_bits_cl0_u64_be(8);
    xu_test_bits_cl0_u64_be(123);
    xu_test_bits_cl0_u64_be(123456);
    xu_test_bits_cl0_u64_be(800000);
    xu_test_bits_cl0_u64_be(31415926);
    xu_test_bits_cl0_u64_be(0xffffffffffffffffull);
    xu_test_bits_cl0_u64_be(0xfffffffffffffff0ull);
    xu_test_bits_cl0_u64_be(0x0fffffffffffffffull);
    xu_test_bits_cl0_u64_be(0xfff00ffffff00fffull);
    xu_test_bits_cl0_u64_be(0x0ffffff00ffffff0ull);
    xu_test_bits_cl0_u64_be(0x000000000000000full);
    xu_test_bits_cl0_u64_be(0xf000000000000000ull);

    xu_trace_i("");
    xu_test_bits_cl0_u64_le(0);
    xu_test_bits_cl0_u64_le(1);
    xu_test_bits_cl0_u64_le(2);
    xu_test_bits_cl0_u64_le(8);
    xu_test_bits_cl0_u64_le(123);
    xu_test_bits_cl0_u64_le(123456);
    xu_test_bits_cl0_u64_le(800000);
    xu_test_bits_cl0_u64_le(31415926);
    xu_test_bits_cl0_u64_le(0xffffffffffffffffull);
    xu_test_bits_cl0_u64_le(0xfffffffffffffff0ull);
    xu_test_bits_cl0_u64_le(0x0fffffffffffffffull);
    xu_test_bits_cl0_u64_le(0xfff00ffffff00fffull);
    xu_test_bits_cl0_u64_le(0x0ffffff00ffffff0ull);
    xu_test_bits_cl0_u64_le(0x000000000000000full);
    xu_test_bits_cl0_u64_le(0xf000000000000000ull);
#endif

#if 1
    xu_trace_i("");
    xu_test_bits_cl1_u32_be(0);
    xu_test_bits_cl1_u32_be(1);
    xu_test_bits_cl1_u32_be(2);
    xu_test_bits_cl1_u32_be(8);
    xu_test_bits_cl1_u32_be(123);
    xu_test_bits_cl1_u32_be(123456);
    xu_test_bits_cl1_u32_be(800000);
    xu_test_bits_cl1_u32_be(31415926);
    xu_test_bits_cl1_u32_be(0xffffffff);
    xu_test_bits_cl1_u32_be(0xfffffff0);
    xu_test_bits_cl1_u32_be(0x0fffffff);
    xu_test_bits_cl1_u32_be(0xfff00fff);
    xu_test_bits_cl1_u32_be(0x0ffffff0);
    xu_test_bits_cl1_u32_be(0x0000000f);
    xu_test_bits_cl1_u32_be(0xf0000000);

    xu_trace_i("");
    xu_test_bits_cl1_u32_le(0);
    xu_test_bits_cl1_u32_le(1);
    xu_test_bits_cl1_u32_le(2);
    xu_test_bits_cl1_u32_le(8);
    xu_test_bits_cl1_u32_le(123);
    xu_test_bits_cl1_u32_le(123456);
    xu_test_bits_cl1_u32_le(800000);
    xu_test_bits_cl1_u32_le(31415926);
    xu_test_bits_cl1_u32_le(0xffffffff);
    xu_test_bits_cl1_u32_le(0xfffffff0);
    xu_test_bits_cl1_u32_le(0x0fffffff);
    xu_test_bits_cl1_u32_le(0xfff00fff);
    xu_test_bits_cl1_u32_le(0x0ffffff0);
    xu_test_bits_cl1_u32_le(0x0000000f);
    xu_test_bits_cl1_u32_le(0xf0000000);

    xu_trace_i("");
    xu_test_bits_cl1_u64_be(0);
    xu_test_bits_cl1_u64_be(1);
    xu_test_bits_cl1_u64_be(2);
    xu_test_bits_cl1_u64_be(8);
    xu_test_bits_cl1_u64_be(123);
    xu_test_bits_cl1_u64_be(123456);
    xu_test_bits_cl1_u64_be(800000);
    xu_test_bits_cl1_u64_be(31415926);
    xu_test_bits_cl1_u64_be(0xffffffffffffffffull);
    xu_test_bits_cl1_u64_be(0xfffffffffffffff0ull);
    xu_test_bits_cl1_u64_be(0x0fffffffffffffffull);
    xu_test_bits_cl1_u64_be(0xfff00ffffff00fffull);
    xu_test_bits_cl1_u64_be(0x0ffffff00ffffff0ull);
    xu_test_bits_cl1_u64_be(0x000000000000000full);
    xu_test_bits_cl1_u64_be(0xf000000000000000ull);

    xu_trace_i("");
    xu_test_bits_cl1_u64_le(0);
    xu_test_bits_cl1_u64_le(1);
    xu_test_bits_cl1_u64_le(2);
    xu_test_bits_cl1_u64_le(8);
    xu_test_bits_cl1_u64_le(123);
    xu_test_bits_cl1_u64_le(123456);
    xu_test_bits_cl1_u64_le(800000);
    xu_test_bits_cl1_u64_le(31415926);
    xu_test_bits_cl1_u64_le(0xffffffffffffffffull);
    xu_test_bits_cl1_u64_le(0xfffffffffffffff0ull);
    xu_test_bits_cl1_u64_le(0x0fffffffffffffffull);
    xu_test_bits_cl1_u64_le(0xfff00ffffff00fffull);
    xu_test_bits_cl1_u64_le(0x0ffffff00ffffff0ull);
    xu_test_bits_cl1_u64_le(0x000000000000000full);
    xu_test_bits_cl1_u64_le(0xf000000000000000ull);
#endif

#if 1
    xu_trace_i("");
    xu_test_bits_cb0_u32(0);
    xu_test_bits_cb0_u32(1);
    xu_test_bits_cb0_u32(2);
    xu_test_bits_cb0_u32(8);
    xu_test_bits_cb0_u32(123);
    xu_test_bits_cb0_u32(123456);
    xu_test_bits_cb0_u32(800000);
    xu_test_bits_cb0_u32(31415926);
    xu_test_bits_cb0_u32(0xffffffff);
    xu_test_bits_cb0_u32(0xfffffff0);
    xu_test_bits_cb0_u32(0x0fffffff);
    xu_test_bits_cb0_u32(0xfff00fff);
    xu_test_bits_cb0_u32(0x0ffffff0);
    xu_test_bits_cb0_u32(0x0000000f);
    xu_test_bits_cb0_u32(0xf0000000);

    xu_trace_i("");
    xu_test_bits_cb0_u64(0);
    xu_test_bits_cb0_u64(1);
    xu_test_bits_cb0_u64(2);
    xu_test_bits_cb0_u64(8);
    xu_test_bits_cb0_u64(123);
    xu_test_bits_cb0_u64(123456);
    xu_test_bits_cb0_u64(800000);
    xu_test_bits_cb0_u64(31415926);
    xu_test_bits_cb0_u64(0xffffffffffffffffull);
    xu_test_bits_cb0_u64(0xfffffffffffffff0ull);
    xu_test_bits_cb0_u64(0x0fffffffffffffffull);
    xu_test_bits_cb0_u64(0xfff00ffffff00fffull);
    xu_test_bits_cb0_u64(0x0ffffff00ffffff0ull);
    xu_test_bits_cb0_u64(0x000000000000000full);
    xu_test_bits_cb0_u64(0xf000000000000000ull);
#endif

#if 1
    xu_trace_i("");
    xu_test_bits_cb1_u32(0);
    xu_test_bits_cb1_u32(1);
    xu_test_bits_cb1_u32(2);
    xu_test_bits_cb1_u32(8);
    xu_test_bits_cb1_u32(123);
    xu_test_bits_cb1_u32(123456);
    xu_test_bits_cb1_u32(800000);
    xu_test_bits_cb1_u32(31415926);
    xu_test_bits_cb1_u32(0xffffffff);
    xu_test_bits_cb1_u32(0xfffffff0);
    xu_test_bits_cb1_u32(0x0fffffff);
    xu_test_bits_cb1_u32(0xfff00fff);
    xu_test_bits_cb1_u32(0x0ffffff0);
    xu_test_bits_cb1_u32(0x0000000f);
    xu_test_bits_cb1_u32(0xf0000000);

    xu_trace_i("");
    xu_test_bits_cb1_u64(0);
    xu_test_bits_cb1_u64(1);
    xu_test_bits_cb1_u64(2);
    xu_test_bits_cb1_u64(8);
    xu_test_bits_cb1_u64(123);
    xu_test_bits_cb1_u64(123456);
    xu_test_bits_cb1_u64(800000);
    xu_test_bits_cb1_u64(31415926);
    xu_test_bits_cb1_u64(0xffffffffffffffffull);
    xu_test_bits_cb1_u64(0xfffffffffffffff0ull);
    xu_test_bits_cb1_u64(0x0fffffffffffffffull);
    xu_test_bits_cb1_u64(0xfff00ffffff00fffull);
    xu_test_bits_cb1_u64(0x0ffffff00ffffff0ull);
    xu_test_bits_cb1_u64(0x000000000000000full);
    xu_test_bits_cb1_u64(0xf000000000000000ull);
#endif

#if 1
    xu_trace_i("");
    xu_test_bits_fb0_u32_be(0);
    xu_test_bits_fb0_u32_be(1);
    xu_test_bits_fb0_u32_be(2);
    xu_test_bits_fb0_u32_be(8);
    xu_test_bits_fb0_u32_be(123);
    xu_test_bits_fb0_u32_be(123456);
    xu_test_bits_fb0_u32_be(800000);
    xu_test_bits_fb0_u32_be(31415926);
    xu_test_bits_fb0_u32_be(0xffffffff);
    xu_test_bits_fb0_u32_be(0xfffffff0);
    xu_test_bits_fb0_u32_be(0x0fffffff);
    xu_test_bits_fb0_u32_be(0xfff00fff);
    xu_test_bits_fb0_u32_be(0x0ffffff0);
    xu_test_bits_fb0_u32_be(0x0000000f);
    xu_test_bits_fb0_u32_be(0xf0000000);

    xu_trace_i("");
    xu_test_bits_fb0_u32_le(0);
    xu_test_bits_fb0_u32_le(1);
    xu_test_bits_fb0_u32_le(2);
    xu_test_bits_fb0_u32_le(8);
    xu_test_bits_fb0_u32_le(123);
    xu_test_bits_fb0_u32_le(123456);
    xu_test_bits_fb0_u32_le(800000);
    xu_test_bits_fb0_u32_le(31415926);
    xu_test_bits_fb0_u32_le(0xffffffff);
    xu_test_bits_fb0_u32_le(0xfffffff0);
    xu_test_bits_fb0_u32_le(0x0fffffff);
    xu_test_bits_fb0_u32_le(0xfff00fff);
    xu_test_bits_fb0_u32_le(0x0ffffff0);
    xu_test_bits_fb0_u32_le(0x0000000f);
    xu_test_bits_fb0_u32_le(0xf0000000);

    xu_trace_i("");
    xu_test_bits_fb0_u64_be(0);
    xu_test_bits_fb0_u64_be(1);
    xu_test_bits_fb0_u64_be(2);
    xu_test_bits_fb0_u64_be(8);
    xu_test_bits_fb0_u64_be(123);
    xu_test_bits_fb0_u64_be(123456);
    xu_test_bits_fb0_u64_be(800000);
    xu_test_bits_fb0_u64_be(31415926);
    xu_test_bits_fb0_u64_be(0xffffffffffffffffull);
    xu_test_bits_fb0_u64_be(0xfffffffffffffff0ull);
    xu_test_bits_fb0_u64_be(0x0fffffffffffffffull);
    xu_test_bits_fb0_u64_be(0xfff00ffffff00fffull);
    xu_test_bits_fb0_u64_be(0x0ffffff00ffffff0ull);
    xu_test_bits_fb0_u64_be(0x000000000000000full);
    xu_test_bits_fb0_u64_be(0xf000000000000000ull);

    xu_trace_i("");
    xu_test_bits_fb0_u64_le(0);
    xu_test_bits_fb0_u64_le(1);
    xu_test_bits_fb0_u64_le(2);
    xu_test_bits_fb0_u64_le(8);
    xu_test_bits_fb0_u64_le(123);
    xu_test_bits_fb0_u64_le(123456);
    xu_test_bits_fb0_u64_le(800000);
    xu_test_bits_fb0_u64_le(31415926);
    xu_test_bits_fb0_u64_le(0xffffffffffffffffull);
    xu_test_bits_fb0_u64_le(0xfffffffffffffff0ull);
    xu_test_bits_fb0_u64_le(0x0fffffffffffffffull);
    xu_test_bits_fb0_u64_le(0xfff00ffffff00fffull);
    xu_test_bits_fb0_u64_le(0x0ffffff00ffffff0ull);
    xu_test_bits_fb0_u64_le(0x000000000000000full);
    xu_test_bits_fb0_u64_le(0xf000000000000000ull);
#endif

#if 1
    xu_trace_i("");
    xu_test_bits_fb1_u32_be(0);
    xu_test_bits_fb1_u32_be(1);
    xu_test_bits_fb1_u32_be(2);
    xu_test_bits_fb1_u32_be(8);
    xu_test_bits_fb1_u32_be(123);
    xu_test_bits_fb1_u32_be(123456);
    xu_test_bits_fb1_u32_be(800000);
    xu_test_bits_fb1_u32_be(31415926);
    xu_test_bits_fb1_u32_be(0xffffffff);
    xu_test_bits_fb1_u32_be(0xfffffff0);
    xu_test_bits_fb1_u32_be(0x0fffffff);
    xu_test_bits_fb1_u32_be(0xfff00fff);
    xu_test_bits_fb1_u32_be(0x0ffffff0);
    xu_test_bits_fb1_u32_be(0x0000000f);
    xu_test_bits_fb1_u32_be(0xf0000000);

    xu_trace_i("");
    xu_test_bits_fb1_u32_le(0);
    xu_test_bits_fb1_u32_le(1);
    xu_test_bits_fb1_u32_le(2);
    xu_test_bits_fb1_u32_le(8);
    xu_test_bits_fb1_u32_le(123);
    xu_test_bits_fb1_u32_le(123456);
    xu_test_bits_fb1_u32_le(800000);
    xu_test_bits_fb1_u32_le(31415926);
    xu_test_bits_fb1_u32_le(0xffffffff);
    xu_test_bits_fb1_u32_le(0xfffffff0);
    xu_test_bits_fb1_u32_le(0x0fffffff);
    xu_test_bits_fb1_u32_le(0xfff00fff);
    xu_test_bits_fb1_u32_le(0x0ffffff0);
    xu_test_bits_fb1_u32_le(0x0000000f);
    xu_test_bits_fb1_u32_le(0xf0000000);

    xu_trace_i("");
    xu_test_bits_fb1_u64_be(0);
    xu_test_bits_fb1_u64_be(1);
    xu_test_bits_fb1_u64_be(2);
    xu_test_bits_fb1_u64_be(8);
    xu_test_bits_fb1_u64_be(123);
    xu_test_bits_fb1_u64_be(123456);
    xu_test_bits_fb1_u64_be(800000);
    xu_test_bits_fb1_u64_be(31415926);
    xu_test_bits_fb1_u64_be(0xffffffffffffffffull);
    xu_test_bits_fb1_u64_be(0xfffffffffffffff0ull);
    xu_test_bits_fb1_u64_be(0x0fffffffffffffffull);
    xu_test_bits_fb1_u64_be(0xfff00ffffff00fffull);
    xu_test_bits_fb1_u64_be(0x0ffffff00ffffff0ull);
    xu_test_bits_fb1_u64_be(0x000000000000000full);
    xu_test_bits_fb1_u64_be(0xf000000000000000ull);

    xu_trace_i("");
    xu_test_bits_fb1_u64_le(0);
    xu_test_bits_fb1_u64_le(1);
    xu_test_bits_fb1_u64_le(2);
    xu_test_bits_fb1_u64_le(8);
    xu_test_bits_fb1_u64_le(123);
    xu_test_bits_fb1_u64_le(123456);
    xu_test_bits_fb1_u64_le(800000);
    xu_test_bits_fb1_u64_le(31415926);
    xu_test_bits_fb1_u64_le(0xffffffffffffffffull);
    xu_test_bits_fb1_u64_le(0xfffffffffffffff0ull);
    xu_test_bits_fb1_u64_le(0x0fffffffffffffffull);
    xu_test_bits_fb1_u64_le(0xfff00ffffff00fffull);
    xu_test_bits_fb1_u64_le(0x0ffffff00ffffff0ull);
    xu_test_bits_fb1_u64_le(0x000000000000000full);
    xu_test_bits_fb1_u64_le(0xf000000000000000ull);
#endif

    return 0;
}
