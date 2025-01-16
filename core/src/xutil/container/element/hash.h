#pragma once

#include "xutil/prefix.h"

/* compute the uint8 hash
 *
 * @param value     the value
 * @param mask      the mask
 * @param index     the hash func index
 *
 * @return          the hash value
 */
xu_size_t xu_element_hash_uint8(xu_uint8_t value, xu_size_t mask, xu_size_t index);

/* compute the uint16 hash
 *
 * @param value     the value
 * @param mask      the mask
 * @param index     the hash func index
 *
 * @return          the hash value
 */
xu_size_t xu_element_hash_uint16(xu_uint16_t value, xu_size_t mask, xu_size_t index);

/* compute the uint32 hash
 *
 * @param value     the value
 * @param mask      the mask
 * @param index     the hash func index
 *
 * @return          the hash value
 */
xu_size_t xu_element_hash_uint32(xu_uint32_t value, xu_size_t mask, xu_size_t index);

/* compute the uint64 hash
 *
 * @param value     the value
 * @param mask      the mask
 * @param index     the hash func index
 *
 * @return          the hash value
 */
xu_size_t xu_element_hash_uint64(xu_uint64_t value, xu_size_t mask, xu_size_t index);

/* compute the data hash
 *
 * @param data      the data
 * @param size      the size
 * @param mask      the mask
 * @param index     the hash func index
 *
 * @return          the hash value
 */
xu_size_t xu_element_hash_data(xu_byte_t const* data, xu_size_t size, xu_size_t mask, xu_size_t index);

/* compute the cstring hash
 *
 * @param cstr      the cstring
 * @param mask      the mask
 * @param index     the hash func index
 *
 * @return          the hash value
 */
xu_size_t xu_element_hash_cstr(xu_char_t const* cstr, xu_size_t mask, xu_size_t index);
