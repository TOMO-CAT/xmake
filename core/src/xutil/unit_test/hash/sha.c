#include "xutil/unit_test/ut.h"

static xu_void_t xu_test_sha(xu_size_t mode, xu_char_t const* data)
{
    xu_byte_t ob[32];
    xu_size_t on = xu_sha_make(mode, (xu_byte_t const*)data, xu_strlen(data), ob, 32);
    xu_assert_and_check_return((on << 3) == mode);

    xu_size_t i        = 0;
    xu_char_t sha[256] = {0};
    for (i = 0; i < on; ++i)
        xu_snprintf(sha + (i << 1), 3, "%02X", ob[i]);
    xu_printf("[sha]: %d = %s\n", mode, sha);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_sha_main(xu_int_t argc, xu_char_t** argv)
{
    xu_test_sha(XU_SHA_MODE_SHA1_160, argv[1]);
    xu_test_sha(XU_SHA_MODE_SHA2_224, argv[1]);
    xu_test_sha(XU_SHA_MODE_SHA2_256, argv[1]);

    return 0;
}
