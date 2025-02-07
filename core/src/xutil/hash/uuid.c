#include "xutil/hash/uuid.h"
#include "xutil/hash/md5.h"
#include "xutil/libc/libc.h"
#include "xutil/libc/stdlib/stdlib.h"
#include "xutil/math/random/random.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * private implementation
 */

// http://xorshift.di.unimi.it/xorshift128plus.c
static xu_uint64_t xu_uuid4_xorshift128plus(xu_uint64_t* s)
{
    xu_uint64_t       s1 = s[0];
    xu_uint64_t const s0 = s[1];
    s[0]                 = s0;
    s1 ^= s1 << 23;
    s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    return s[1] + s0;
}
static xu_bool_t xu_uuid4_generate(xu_byte_t uuid[16], xu_uint64_t seed[2])
{
    // init seed data
    union
    {
        xu_byte_t   b[16];
        xu_uint64_t word[2];
    } s;
    s.word[0] = xu_uuid4_xorshift128plus(seed);
    s.word[1] = xu_uuid4_xorshift128plus(seed);

    /* generate uuid
     *
     * t: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
     *    xxxxxxxxxxxx4xxxyxxxxxxxxxxxxxxx
     */
    xu_int_t i  = 0;
    xu_int_t n  = 0;
    xu_int_t t  = 0;
    xu_int_t c1 = 0;
    xu_int_t c2 = 0;
    for (t = 0; t < 32; t++)
    {
        n = s.b[i >> 1];
        n = (i & 1) ? (n >> 4) : (n & 0xf);
        if (t == 16) // y
        {
            c2 = (n & 0x3) + 8;
            i++;
        }
        else if (t == 12)
            c2 = 4; // 4
        else        // x
        {
            c2 = n;
            i++;
        }
        if (t & 1) uuid[t >> 1] = (xu_byte_t)(c1 * 16 + c2);
        c1 = c2;
    }
    return xu_true;
}

/* *******************************************************
 * implementation
 */
xu_bool_t        xu_uuid_make(xu_byte_t uuid[16], xu_char_t const* name) { return xu_uuid4_make(uuid, name); }
xu_char_t const* xu_uuid_make_cstr(xu_char_t uuid_cstr[37], xu_char_t const* name)
{
    return xu_uuid4_make_cstr(uuid_cstr, name);
}
xu_bool_t xu_uuid4_make(xu_byte_t uuid[16], xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(uuid, xu_false);

    // init seed
    xu_uint64_t seed[2];
    if (name)
    {
        xu_assert_static(sizeof(seed) == 16);
        xu_md5_make((xu_byte_t const*)name, xu_strlen(name), (xu_byte_t*)seed, 16);
    }
    else
    {
        // disable pseudo random
        xu_random_reset(xu_false);
        seed[0] = (xu_uint64_t)xu_random();
        seed[1] = (xu_uint64_t)xu_random();
    }

    // generate uuid v4
    return xu_uuid4_generate(uuid, seed);
}
xu_char_t const* xu_uuid4_make_cstr(xu_char_t uuid_cstr[37], xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(uuid_cstr, xu_null);

    // make uuid bytes
    xu_byte_t uuid[16];
    if (!xu_uuid4_make(uuid, name)) return xu_null;

    // make uuid string
    xu_long_t size = xu_snprintf(uuid_cstr, 37, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                                 uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8],
                                 uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    xu_assert_and_check_return_val(size == 36, xu_null);

    // end
    uuid_cstr[36] = '\0';

    // ok
    return uuid_cstr;
}
