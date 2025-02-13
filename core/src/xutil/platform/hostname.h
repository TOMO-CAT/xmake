#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * interfaces
 */

/*! the host name
 *
 * @param name  the hostname data
 * @param size  the hostname size
 *
 * @return      xu_true or xu_false
 */
xu_bool_t xu_hostname(xu_char_t* name, xu_size_t size);
