#include "xutil/prefix.h"
#include <stdlib.h>

/* *******************************************************
 * implementation
 */

xu_void_t xu_random_seed(xu_size_t seed) { srandom(seed); }
xu_long_t xu_random_value() { return (xu_long_t)random(); }
