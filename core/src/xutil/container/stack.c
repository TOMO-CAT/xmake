#include "xutil/container/stack.h"
#include "xutil/libc/libc.h"
#include "xutil/memory/memory.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * interfaces
 */

xu_stack_ref_t xu_stack_init(xu_size_t grow, xu_element_t element)
{
    return (xu_stack_ref_t)xu_vector_init(grow, element);
}
xu_void_t xu_stack_exit(xu_stack_ref_t self) { xu_vector_exit((xu_vector_ref_t)self); }
xu_void_t xu_stack_clear(xu_stack_ref_t self) { xu_vector_clear((xu_vector_ref_t)self); }
xu_void_t xu_stack_copy(xu_stack_ref_t self, xu_stack_ref_t copy) { xu_vector_copy((xu_vector_ref_t)self, copy); }
xu_void_t xu_stack_put(xu_stack_ref_t self, xu_cpointer_t data) { xu_vector_insert_tail((xu_vector_ref_t)self, data); }
xu_void_t xu_stack_pop(xu_stack_ref_t self) { xu_vector_remove_last((xu_vector_ref_t)self); }
xu_pointer_t xu_stack_top(xu_stack_ref_t self) { return xu_vector_last((xu_vector_ref_t)self); }
xu_pointer_t xu_stack_head(xu_stack_ref_t self) { return xu_vector_head((xu_vector_ref_t)self); }
xu_pointer_t xu_stack_last(xu_stack_ref_t self) { return xu_vector_last((xu_vector_ref_t)self); }
xu_size_t    xu_stack_size(xu_stack_ref_t self) { return xu_vector_size((xu_vector_ref_t)self); }
xu_size_t    xu_stack_maxn(xu_stack_ref_t self) { return xu_vector_maxn((xu_vector_ref_t)self); }
#ifdef __xu_debug__
xu_void_t xu_stack_dump(xu_stack_ref_t self) { xu_vector_dump((xu_vector_ref_t)self); }
#endif
