#pragma once

#include "xutil/prefix.h"

/*! get the last system error state
 *
 * @note the error code see state.h
 *
 * @return          the last system error state
 */
xu_size_t xu_syserror_state(xu_noarg_t);
