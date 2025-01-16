#pragma once

#include "xutil/prefix.h"

/*! make blizzard hash
 *
 * Blizzard One-Way Hash algorithm from MPQ
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the blizzard value
 */
xu_size_t xu_blizzard_make(xu_byte_t const* data, xu_size_t size, xu_size_t seed);

/*! make blizzard hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the blizzard value
 */
xu_size_t xu_blizzard_make_from_cstr(xu_char_t const* cstr, xu_size_t seed);
