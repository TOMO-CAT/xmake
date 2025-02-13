#include "xutil/unit_test/ut.h"

/* *******************************************************
 * types
 */

// the hash32 entry type
typedef struct __xu_demo_hash32_entry_t
{
    // the hash name
    xu_char_t const* name;

    // the hash function
    xu_uint32_t (*hash)(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed);

} xu_demo_hash32_entry_t, *xu_demo_hash32_entry_ref_t;

/* *******************************************************
 * wrapers
 */
static xu_uint32_t xu_demo_rs_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
    return (xu_uint32_t)xu_rs_make(data, size, seed);
}
static xu_uint32_t xu_demo_ap_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
    return (xu_uint32_t)xu_ap_make(data, size, seed);
}
static xu_uint32_t xu_demo_djb2_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
    return (xu_uint32_t)xu_djb2_make(data, size, seed);
}
static xu_uint32_t xu_demo_sdbm_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
    return (xu_uint32_t)xu_sdbm_make(data, size, seed);
}
static xu_uint32_t xu_demo_bkdr_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
    return (xu_uint32_t)xu_bkdr_make(data, size, seed);
}
static xu_uint32_t xu_demo_murmur_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
    return (xu_uint32_t)xu_murmur_make(data, size, seed);
}
static xu_uint32_t xu_demo_blizzard_make(xu_byte_t const* data, xu_size_t size, xu_uint32_t seed)
{
    return (xu_uint32_t)xu_blizzard_make(data, size, seed);
}

/* *******************************************************
 * globals
 */
static xu_demo_hash32_entry_t g_hash32_entries[] = {{"fnv32   ", xu_fnv32_make},
                                                    {"fnv32-1a", xu_fnv32_1a_make},
                                                    {"rs      ", xu_demo_rs_make},
                                                    {"ap      ", xu_demo_ap_make},
                                                    {"djb2    ", xu_demo_djb2_make},
                                                    {"sdbm    ", xu_demo_sdbm_make},
                                                    {"adler32 ", xu_adler32_make},
                                                    {"crc32   ", xu_crc32_make},
                                                    {"crc32-le", xu_crc32_le_make},
                                                    {"bkdr    ", xu_demo_bkdr_make},
                                                    {"murmur  ", xu_demo_murmur_make},
                                                    {"blizzard", xu_demo_blizzard_make},
                                                    {xu_null, xu_null}};

/* *******************************************************
 * test
 */
static xu_void_t xu_demo_hash32_test()
{
    // init data
    xu_size_t  size = 1024 * 1024;
    xu_byte_t* data = xu_malloc_bytes(size);
    xu_assert_and_check_return(data);

    // make data
    xu_size_t i = 0;
    for (i = 0; i < size; i++)
        data[i] = (xu_byte_t)xu_random_range(0, 0xff);

    // done (1M)
    xu_demo_hash32_entry_ref_t entry = g_hash32_entries;
    for (; entry && entry->name; entry++)
    {
        __xu_volatile__ xu_uint32_t v = 0;
        __xu_volatile__ xu_uint32_t n = 1000000;
        __xu_volatile__ xu_hong_t   t = xu_mclock();
        while (n--)
        {
            v = entry->hash(data, 1024, n);
        }
        t = xu_mclock() - t;

        // trace
        xu_trace_i("[hash(1K)]: %s: %08x %ld ms", entry->name, v, t);
    }

    // trace
    xu_trace_i("");

    // done (1M)
    entry = g_hash32_entries;
    for (; entry && entry->name; entry++)
    {
        __xu_volatile__ xu_uint32_t v = 0;
        __xu_volatile__ xu_uint32_t n = 1000;
        __xu_volatile__ xu_hong_t   t = xu_mclock();
        while (n--)
        {
            v = entry->hash(data, size, n);
        }
        t = xu_mclock() - t;

        // trace
        xu_trace_i("[hash(1M)]: %s: %08x %ld ms", entry->name, v, t);
    }

    // exit data
    xu_free(data);
}

/* *******************************************************
 */
xu_int_t xu_ut_hash_benchmark_main(xu_int_t argc, xu_char_t** argv)
{
    xu_demo_hash32_test();
    return 0;
}
