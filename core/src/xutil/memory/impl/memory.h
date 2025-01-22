#include "xutil/prefix.h"

/* *******************************************************
 *  includes
 */
#include "xutil/macros/type.h"
#include "xutil/memory/allocator.h"

/* init memory environment
 *
 * @param allocator     the allocator
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_memory_init_env(xu_allocator_ref_t allocator);

// exit memory environment
xu_void_t xu_memory_exit_env(xu_noarg_t);
