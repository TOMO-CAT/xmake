#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_container_hash_set_main(xu_int_t argc, xu_char_t** argv)
{
    // init hash
    xu_hash_set_ref_t hash = xu_hash_set_init(8, xu_element_str(xu_true));
    if (hash)
    {
        xu_hash_set_insert(hash, "1");
        xu_hash_set_insert(hash, "9");
        xu_hash_set_insert(hash, "4");
        xu_hash_set_insert(hash, "6");
        xu_hash_set_insert(hash, "8");
        xu_hash_set_insert(hash, "2");
        xu_hash_set_insert(hash, "3");
        xu_hash_set_insert(hash, "5");
        xu_hash_set_insert(hash, "7");
        xu_for_all_if(xu_char_t const*, s, hash, s) { xu_trace_i("%s", s); }
        xu_hash_set_exit(hash);
    }
    return 0;
}
