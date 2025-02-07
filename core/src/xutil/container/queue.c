#include "xutil/container/queue.h"

/* *******************************************************
 * interfaces
 */
xu_queue_ref_t xu_queue_init(xu_size_t grow, xu_element_t element)
{
    return (xu_queue_ref_t)xu_single_list_init(grow, element);
}
xu_void_t xu_queue_exit(xu_queue_ref_t queue)
{
    xu_single_list_exit((xu_single_list_ref_t)queue);
}
xu_void_t xu_queue_clear(xu_queue_ref_t queue)
{
    xu_single_list_clear((xu_single_list_ref_t)queue);
}
xu_void_t xu_queue_put(xu_queue_ref_t queue, xu_cpointer_t data)
{
    xu_single_list_insert_tail((xu_single_list_ref_t)queue, data);
}
xu_void_t xu_queue_pop(xu_queue_ref_t queue)
{
    xu_single_list_remove_head((xu_single_list_ref_t)queue);
}
xu_pointer_t xu_queue_get(xu_queue_ref_t queue)
{
    return xu_queue_head(queue);
}
xu_pointer_t xu_queue_head(xu_queue_ref_t queue)
{
    return xu_single_list_head((xu_single_list_ref_t)queue);
}
xu_pointer_t xu_queue_last(xu_queue_ref_t queue)
{
    return xu_single_list_last((xu_single_list_ref_t)queue);
}
xu_size_t xu_queue_size(xu_queue_ref_t queue)
{
    return xu_single_list_size((xu_single_list_ref_t)queue);
}
xu_size_t xu_queue_maxn(xu_queue_ref_t queue)
{
    return xu_single_list_maxn((xu_single_list_ref_t)queue);
}
xu_bool_t xu_queue_full(xu_queue_ref_t queue)
{
    return (xu_single_list_size((xu_single_list_ref_t)queue) < xu_single_list_maxn((xu_single_list_ref_t)queue))? xu_false : xu_true;
}
xu_bool_t xu_queue_null(xu_queue_ref_t queue)
{
    return xu_single_list_size((xu_single_list_ref_t)queue)? xu_false : xu_true;
}
#ifdef __xu_debug__
xu_void_t xu_queue_dump(xu_queue_ref_t queue)
{
    xu_single_list_dump((xu_single_list_ref_t)queue);
}
#endif
