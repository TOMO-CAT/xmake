#pragma once

/* *******************************************************
 * includes
 */
#include "xutil/prefix.h"

/* *******************************************************
 * interfaces
 */

/*! make an uuid
 *
 * @param uuid      the uuid output buffer
 * @param name      we only generate it using a simple hashing function for speed if name is supplied
 *
 * @return          xu_true or xu_false
 */
xu_bool_t           xu_uuid_make(xu_byte_t uuid[16], xu_char_t const* name);

/*! make an uuid string
 *
 * @param uuid_cstr the uuid output c-string
 * @param name      we only generate it using a simple hashing function for speed if name is supplied
 *
 * @return          the uuid c-string or xu_null
 */
xu_char_t const*    xu_uuid_make_cstr(xu_char_t uuid_cstr[37], xu_char_t const* name);

/*! make an uuid (version 4)
 *
 * A version 4 UUID is defined in RFC 4122: 128 randomly-generated bits with six bits at certain positions set to particular values. For example,
 *
 * AA97B177-9383-4934-8543-0F91A7A02836
 *               ^    ^
 *               1    2
 * The digit at position 1 above is always "4" and the digit at position 2 is always one of "8", "9", "A" or "B".
 *
 * @see
 * - https://www.cryptosys.net/pki/uuid-rfc4122.html
 * - https://tools.ietf.org/html/rfc4122
 *
 * @param uuid      the uuid output buffer
 * @param name      we only generate it using a simple hashing function for speed if name is supplied
 *
 * @return          xu_true or xu_false
 */
xu_bool_t           xu_uuid4_make(xu_byte_t uuid[16], xu_char_t const* name);

/*! make an uuid string (version 4)
 *
 * @param uuid_cstr the uuid output c-string
 * @param name      we only generate it using a simple hashing function for speed if name is supplied
 *
 * @return          the uuid c-string or xu_null
 */
xu_char_t const*    xu_uuid4_make_cstr(xu_char_t uuid_cstr[37], xu_char_t const* name);
