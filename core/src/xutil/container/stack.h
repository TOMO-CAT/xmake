#pragma once

/* *******************************************************
 * includes
 */
#include "xutil/container/element.h"
#include "xutil/container/vector.h"

/* *******************************************************
 * types
 */

/*! the stack ref type
 *
 * <pre>
 * stack: |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||------|
 *       head                                                           last    tail
 *
 * stack: |||||||||||||||||||||||||||||||||||||||||------|
 *       head                                   last    tail
 *
 * performance:
 *
 * push:    fast
 * pop:     fast
 *
 * iterator:
 * next:    fast
 * prev:    fast
 * </pre>
 *
 * @note the itor of the same item is fixed
 *
 */
typedef xu_vector_ref_t xu_stack_ref_t;

/* *******************************************************
 * interfaces
 */

/*! init stack
 *
 * @param grow      the item grow
 * @param element   the element
 *
 * @return          the stack
 */
xu_stack_ref_t xu_stack_init(xu_size_t grow, xu_element_t element);

/*! exit stack
 *
 * @param stack     the stack
 */
xu_void_t xu_stack_exit(xu_stack_ref_t stack);

/*! the stack head item
 *
 * @param stack     the stack
 *
 * @return          the head item
 */
xu_pointer_t xu_stack_head(xu_stack_ref_t stack);

/*! the stack last item
 *
 * @param stack     the stack
 *
 * @return          the last item
 */
xu_pointer_t xu_stack_last(xu_stack_ref_t stack);

/*! clear the stack
 *
 * @param stack     the stack
 */
xu_void_t xu_stack_clear(xu_stack_ref_t stack);

/*! copy the stack
 *
 * @param stack     the stack
 * @param copy      the copied stack
 */
xu_void_t xu_stack_copy(xu_stack_ref_t stack, xu_stack_ref_t copy);

/*! put the stack item
 *
 * @param stack     the stack
 * @param data      the item data
 */
xu_void_t xu_stack_put(xu_stack_ref_t stack, xu_cpointer_t data);

/*! pop the stack item
 *
 * @param stack     the stack
 */
xu_void_t xu_stack_pop(xu_stack_ref_t stack);

/*! the stack top item
 *
 * @param stack     the stack
 *
 * @return          the stack top item
 */
xu_pointer_t xu_stack_top(xu_stack_ref_t stack);

/*! the stack size
 *
 * @param stack     the stack
 *
 * @return          the stack size
 */
xu_size_t xu_stack_size(xu_stack_ref_t stack);

/*! the stack maxn
 *
 * @param stack     the stack
 *
 * @return          the stack maxn
 */
xu_size_t xu_stack_maxn(xu_stack_ref_t stack);

#ifdef __xu_debug__
/*! dump stack
 *
 * @param stack     the stack
 */
xu_void_t xu_stack_dump(xu_stack_ref_t stack);
#endif
