#include "xutil/math/random/random.h"
#include "xutil/libc/stdlib/stdlib.h"

xu_void_t xu_srandom(xu_size_t seed) { xu_random_seed(seed); }
xu_long_t xu_random() { return xu_random_value(); }
