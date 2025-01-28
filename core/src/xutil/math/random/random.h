#pragma once

#include "xutil/math/random/linear.h"
#include "xutil/prefix.h"

/*! set the random seed
 *
 * @param seed      the random seed
 */
xu_void_t xu_random_seed(xu_size_t seed);

/*! reset value using the initial seed
 *
 * @param pseudo    reset to the pseudo random?
 */
xu_void_t xu_random_reset(xu_bool_t pseudo);

/*! generate the random value
 *
 * it will generate a pseudo-random sequence if the seed is not modified manually.
 *
 * @return          the random value
 */
xu_long_t xu_random_value(xu_noarg_t);

/*! generate the random with range: [begin, end)
 *
 * @param begin     the begin value
 * @param end       the end value
 *
 * @return          the random value
 */
xu_long_t xu_random_range(xu_long_t begin, xu_long_t end);

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
/*! generate the float random with range: [begin, end)
 *
 * @param begin     the begin value
 * @param end       the end value
 *
 * @return          the random value
 */
xu_float_t xu_random_rangef(xu_float_t begin, xu_float_t end);
#endif
