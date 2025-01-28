#pragma once

#include "xutil/prefix.h"

/*! set the linear random seed
 *
 * @param seed      the random seed
 */
xu_void_t xu_random_linear_seed(xu_size_t seed);

/*! generate the linear random value
 *
 * @return          the random value
 */
xu_long_t xu_random_linear_value(xu_noarg_t);
