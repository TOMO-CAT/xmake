#include "xutil/container/element/hash.h"
#include "xutil/hash/adler32.h"
#include "xutil/hash/ap.h"
#include "xutil/hash/bkdr.h"
#include "xutil/hash/blizzard.h"
#include "xutil/hash/crc32.h"
#include "xutil/hash/djb2.h"
#include "xutil/hash/fnv32.h"
#include "xutil/hash/md5.h"
#include "xutil/hash/murmur.h"
#include "xutil/hash/rs.h"
#include "xutil/hash/sdbm.h"
#include "xutil/hash/sha.h"
#include "xutil/libc/string/string.h"
#include "xutil/utils/trace.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * data hash implementation
 * *******************************************************
 */
static xu_size_t xu_element_hash_data_func_0(xu_byte_t const* data, xu_size_t size)
{
    return xu_bkdr_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_1(xu_byte_t const* data, xu_size_t size)
{
    return xu_adler32_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_2(xu_byte_t const* data, xu_size_t size)
{
    return xu_fnv32_1a_make(data, size, 0);
}
#if !defined(__xu_small__) && defined(XU_CONFIG_MODULE_HAVE_HASH)
static xu_size_t xu_element_hash_data_func_3(xu_byte_t const* data, xu_size_t size)
{
    return xu_ap_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_4(xu_byte_t const* data, xu_size_t size)
{
    return xu_murmur_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_5(xu_byte_t const* data, xu_size_t size)
{
    return xu_crc32_le_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_6(xu_byte_t const* data, xu_size_t size)
{
    return xu_fnv32_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_7(xu_byte_t const* data, xu_size_t size)
{
    return xu_djb2_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_8(xu_byte_t const* data, xu_size_t size)
{
    return xu_blizzard_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_9(xu_byte_t const* data, xu_size_t size)
{
    return xu_rs_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_10(xu_byte_t const* data, xu_size_t size)
{
    return xu_sdbm_make(data, size, 0);
}
static xu_size_t xu_element_hash_data_func_11(xu_byte_t const* data, xu_size_t size)
{
    // using md5, better but too slower
    xu_byte_t b[16] = {0};
    xu_md5_make(data, size, b, 16);
    return xu_bits_get_u32_ne(b);
}
static xu_size_t xu_element_hash_data_func_12(xu_byte_t const* data, xu_size_t size)
{
    // using sha, better but too slower
    xu_byte_t b[32] = {0};
    xu_sha_make(XU_SHA_MODE_SHA1_160, data, size, b, 32);
    return xu_bits_get_u32_ne(b);
}
static xu_size_t xu_element_hash_data_func_13(xu_byte_t const* data, xu_size_t size)
{
    xu_trace_noimpl();
    return 0;
}
static xu_size_t xu_element_hash_data_func_14(xu_byte_t const* data, xu_size_t size)
{
    xu_trace_noimpl();
    return 0;
}
static xu_size_t xu_element_hash_data_func_15(xu_byte_t const* data, xu_size_t size)
{
    xu_trace_noimpl();
    return 0;
}
#endif

/* *******************************************************
 * cstr hash implementation
 * *******************************************************
 */
static xu_size_t xu_element_hash_cstr_func_0(xu_char_t const* data) { return xu_bkdr_make_from_cstr(data, 0); }
static xu_size_t xu_element_hash_cstr_func_1(xu_char_t const* data) { return xu_fnv32_1a_make_from_cstr(data, 0); }

/* *******************************************************
 * uint8 hash implementation
 * *******************************************************
 */

static xu_size_t xu_element_hash_uint8_func_0(xu_uint8_t value) { return (xu_size_t)value; }
static xu_size_t xu_element_hash_uint8_func_1(xu_uint8_t value)
{
    return (xu_size_t)(((xu_uint64_t)(value)*2654435761ul) >> 16);
}

/* *******************************************************
 * uint16 hash implementation
 * *******************************************************
 */

static xu_size_t xu_element_hash_uint16_func_0(xu_uint16_t value)
{
    return (xu_size_t)(((xu_uint64_t)(value)*2654435761ul) >> 16);
}

/* *******************************************************
 * uint32 hash implementation
 * *******************************************************
 */

static xu_size_t xu_element_hash_uint32_func_0(xu_uint32_t value)
{
    return (xu_size_t)(((xu_uint64_t)(value)*2654435761ul) >> 16);
}
static xu_size_t xu_element_hash_uint32_func_1(xu_uint32_t value)
{
    // Bob Jenkins' 32 bit integer hash function
    value = (value + 0x7ed55d16) + (value << 12);
    value = (value ^ 0xc761c23c) ^ (value >> 19);
    value = (value + 0x165667b1) + (value << 5);
    value = (value + 0xd3a2646c) ^ (value << 9);
    value = (value + 0xfd7046c5) + (value << 3);
    value = (value ^ 0xb55a4f09) ^ (value >> 16);
    return value;
}
static xu_size_t xu_element_hash_uint32_func_2(xu_uint32_t value)
{
    // Tomas Wang
    value = ~value + (value << 15);
    value = value ^ (value >> 12);
    value = value + (value << 2);
    value = value ^ (value >> 4);
    value = value * 2057;
    value = value ^ (value >> 16);
    return value;
}

/* *******************************************************
 * uint64 hash implementation
 * *******************************************************
 */

static xu_size_t xu_element_hash_uint64_func_0(xu_uint64_t value) { return (xu_size_t)((value * 2654435761ul) >> 16); }

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_size_t xu_element_hash_uint8(xu_uint8_t value, xu_size_t mask, xu_size_t index)
{
    // check
    xu_assert_and_check_return_val(mask, 0);

    // for optimization
    if (index < 2)
    {
        // the func
        static xu_size_t (*s_func[])(xu_uint8_t) = {xu_element_hash_uint8_func_0, xu_element_hash_uint8_func_1};
        xu_assert_and_check_return_val(index < xu_arrayn(s_func), 0);

        // done
        return s_func[index](value) & mask;
    }

    // using uint32 hash
    xu_uint32_t v = (xu_uint32_t)value;
    return xu_element_hash_uint32(v | (v << 8) | (v << 16) | (v << 24), mask, index - 1);
}
xu_size_t xu_element_hash_uint16(xu_uint16_t value, xu_size_t mask, xu_size_t index)
{
    // check
    xu_assert_and_check_return_val(mask, 0);

    // for optimization
    if (index < 1)
    {
        // the func
        static xu_size_t (*s_func[])(xu_uint16_t) = {xu_element_hash_uint16_func_0};
        xu_assert_and_check_return_val(index < xu_arrayn(s_func), 0);

        // done
        return s_func[index](value) & mask;
    }

    // using uint32 hash
    xu_uint32_t v = (xu_uint32_t)value;
    return xu_element_hash_uint32(v | (v << 16), mask, index);
}
xu_size_t xu_element_hash_uint32(xu_uint32_t value, xu_size_t mask, xu_size_t index)
{
    // check
    xu_assert_and_check_return_val(mask, 0);

    // for optimization
    if (index < 3)
    {
        // the func
        static xu_size_t (*s_func[])(xu_uint32_t) = {xu_element_hash_uint32_func_0, xu_element_hash_uint32_func_1,
                                                     xu_element_hash_uint32_func_2};
        xu_assert_and_check_return_val(index < xu_arrayn(s_func), 0);

        // done
        return s_func[index](value) & mask;
    }

    // done
    return xu_element_hash_data((xu_byte_t const*)&value, sizeof(xu_uint32_t), mask, index - 3);
}
xu_size_t xu_element_hash_uint64(xu_uint64_t value, xu_size_t mask, xu_size_t index)
{
    // for optimization
    if (index < 1)
    {
        // the func
        static xu_size_t (*s_func[])(xu_uint64_t) = {xu_element_hash_uint64_func_0};
        xu_assert_and_check_return_val(index < xu_arrayn(s_func), 0);

        // done
        return s_func[index](value) & mask;
    }

    // using the uint32 hash
    xu_size_t hash0 = xu_element_hash_uint32((xu_uint32_t)value, mask, index);
    xu_size_t hash1 = xu_element_hash_uint32((xu_uint32_t)(value >> 32), mask, index);
    return ((hash0 ^ hash1) & mask);
}
xu_size_t xu_element_hash_data(xu_byte_t const* data, xu_size_t size, xu_size_t mask, xu_size_t index)
{
    // check
    xu_assert_and_check_return_val(data && size && mask, 0);

    // the func
    static xu_size_t (*s_func[])(xu_byte_t const*, xu_size_t) = {
        xu_element_hash_data_func_0,
        xu_element_hash_data_func_1,
        xu_element_hash_data_func_2
#if !defined(__xu_small__) && defined(XU_CONFIG_MODULE_HAVE_HASH)
        ,
        xu_element_hash_data_func_3,
        xu_element_hash_data_func_4,
        xu_element_hash_data_func_5,
        xu_element_hash_data_func_6,
        xu_element_hash_data_func_7,
        xu_element_hash_data_func_8,
        xu_element_hash_data_func_9,
        xu_element_hash_data_func_10,
        xu_element_hash_data_func_11,
        xu_element_hash_data_func_12,
        xu_element_hash_data_func_13,
        xu_element_hash_data_func_14,
        xu_element_hash_data_func_15
#endif
    };
    xu_assert_and_check_return_val(index < xu_arrayn(s_func), 0);

    // done
    return s_func[index](data, size) & mask;
}
xu_size_t xu_element_hash_cstr(xu_char_t const* cstr, xu_size_t mask, xu_size_t index)
{
    // check
    xu_assert_and_check_return_val(cstr && mask, 0);

    // for optimization
    if (index < 2)
    {
        // the func
        static xu_size_t (*s_func[])(xu_char_t const*) = {xu_element_hash_cstr_func_0, xu_element_hash_cstr_func_1};
        xu_assert_and_check_return_val(index < xu_arrayn(s_func), 0);

        // done
        return s_func[index](cstr) & mask;
    }

    // using the data hash
    return xu_element_hash_data((xu_byte_t const*)cstr, xu_strlen(cstr), mask, index);
}
