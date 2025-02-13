#include "xutil/unit_test/ut.h"
#include <string.h>

/* *******************************************************
 * check
 */
static xu_bool_t check_memset_u8(xu_byte_t* dst, xu_uint8_t src, xu_size_t size)
{
    xu_uint8_t* p = (xu_uint8_t*)dst;
    xu_uint8_t* e = p + size;
    while (p < e)
    {
        if (*p != src) return xu_false;
        p++;
    }

    return xu_true;
}
static xu_bool_t check_memset_u16(xu_byte_t* dst, xu_uint16_t src, xu_size_t size)
{
    xu_uint16_t* p = (xu_uint16_t*)dst;
    xu_uint16_t* e = p + size;
    while (p < e)
    {
        if (*p != src) return xu_false;
        p++;
    }

    return xu_true;
}
static xu_bool_t check_memset_u24(xu_byte_t* dst, xu_uint32_t src, xu_size_t size)
{
    xu_byte_t* p = (xu_byte_t*)dst;
    xu_byte_t* e = p + (size * 3);
    while (p < e)
    {
        if ((*((xu_uint32_t*)p) & 0xffffff) != (src & 0xffffff)) return xu_false;
        p += 3;
    }

    return xu_true;
}
static xu_bool_t check_memset_u32(xu_byte_t* dst, xu_uint32_t src, xu_size_t size)
{
    xu_uint32_t* p = (xu_uint32_t*)dst;
    xu_uint32_t* e = p + size;
    while (p < e)
    {
        if (*p != src) return xu_false;
        p++;
    }

    return xu_true;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_memory_memops_main(xu_int_t argc, xu_char_t** argv)
{
    // init
    __xu_volatile__ xu_size_t i  = 0;
    __xu_volatile__ xu_hong_t dt = 0;

    xu_size_t  size = 15 * 1024 * 1024;
    xu_byte_t* data = xu_malloc_bytes(size);

    xu_size_t  size2 = 15 * 1024 * 1024;
    xu_byte_t* data2 = xu_malloc_bytes(size2);

    // test: u8 x 1024
    memset(data, 0, size);

    dt = xu_mclock();
    for (i = 0; i < 1000000; i++)
        xu_memset(data, 0xbe, 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memset[1k]: %lld ms\n", dt);
    if (!check_memset_u8(data, 0xbe, 1024 + 3)) xu_printf("check failed\n");

    // test: u8 x 1024 * 1024
    memset(data, 0, size);

    dt = xu_mclock();
    for (i = 0; i < 1000; i++)
        xu_memset(data, 0xbe, 1024 * 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memset[1m]: %lld ms\n", dt);
    if (!check_memset_u8(data, 0xbe, 1024 * 1024 + 3)) xu_printf("check failed\n");

    // test: u8 x 1024
    memset(data2, 0, size2);

    dt = xu_mclock();
    for (i = 0; i < 1000000; i++)
        xu_memcpy(data2, data, 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memcpy[1k]: %lld ms\n", dt);
    if (!check_memset_u8(data2, 0xbe, 1024 + 3)) xu_printf("check failed\n");

    // test: u8 x 1024 * 1024
    memset(data2, 0, size2);

    dt = xu_mclock();
    for (i = 0; i < 1000; i++)
        xu_memcpy(data2, data, 1024 * 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memcpy[1m]: %lld ms\n", dt);
    if (!check_memset_u8(data2, 0xbe, 1024 * 1024 + 3)) xu_printf("check failed\n");

    // test: u8 x 1024
    dt = xu_mclock();
    for (i = 0; i < 1000000; i++)
        xu_memmov(data + 1024 + 3, data, 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memmov[1k]: %lld ms\n", dt);
    if (!check_memset_u8(data + 1024 + 3, 0xbe, 1024 + 3)) xu_printf("check failed\n");

    // test: u8 x 1024 * 1024
    dt = xu_mclock();
    for (i = 0; i < 1000; i++)
        xu_memmov(data + 1024 * 1024 + 3, data, 1024 * 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memmov[1m]: %lld ms\n", dt);
    if (!check_memset_u8(data + 1024 * 1024 + 3, 0xbe, 1024 * 1024 + 3)) xu_printf("check failed\n");

    // test: u16 x 1024
    memset(data, 0, size);

    dt = xu_mclock();
    for (i = 0; i < 1000000; i++)
        xu_memset_u16(data, 0xbeef, 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memset_u16[1k]: %lld ms\n", dt);
    if (!check_memset_u16(data, 0xbeef, 1024 + 3)) xu_printf("check failed\n");

    // test: u16 x 1024 * 1024
    memset(data, 0, size);

    dt = xu_mclock();
    for (i = 0; i < 1000; i++)
        xu_memset_u16(data, 0xbeef, 1024 * 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memset_u16[1m]: %lld ms\n", dt);
    if (!check_memset_u16(data, 0xbeef, 1024 * 1024 + 3)) xu_printf("check failed\n");

    // test: u24 x 1024
    memset(data, 0, size);

    dt = xu_mclock();
    for (i = 0; i < 1000000; i++)
        xu_memset_u24(data, 0xbeefaa, 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memset_u24[1k]: %lld ms\n", dt);
    if (!check_memset_u24(data, 0xbeefaa, 1024 + 3)) xu_printf("check failed\n");

    // test: u24 x 1024 * 1024
    memset(data, 0, size);

    dt = xu_mclock();
    for (i = 0; i < 1000; i++)
        xu_memset_u24(data, 0xbeefaa, 1024 * 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memset_u24[1m]: %lld ms\n", dt);
    if (!check_memset_u24(data, 0xbeefaa, 1024 * 1024 + 3)) xu_printf("check failed\n");

    // test: u32 x 1024
    memset(data, 0, size);

    dt = xu_mclock();
    for (i = 0; i < 1000000; i++)
        xu_memset_u32(data, 0xbeefbeaf, 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memset_u32[1k]: %lld ms\n", dt);
    if (!check_memset_u32(data, 0xbeefbeaf, 1024 + 3)) xu_printf("check failed\n");

    // test: u32 x 1024 * 1024
    memset(data, 0, size);

    dt = xu_mclock();
    for (i = 0; i < 1000; i++)
        xu_memset_u32(data, 0xbeefbeaf, 1024 * 1024 + 3);
    dt = xu_mclock() - dt;
    xu_printf("memset_u32[1m]: %lld ms\n", dt);
    if (!check_memset_u32(data, 0xbeefbeaf, 1024 * 1024 + 3)) xu_printf("check failed\n");

    return 0;
}
