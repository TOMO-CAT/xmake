#include "xutil/algorithm/find.h"
#include "xutil/algorithm/find_if.h"

/* *******************************************************
 * implementation
 */

xu_size_t xu_find(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_cpointer_t value)
{
    return xu_find_if(iterator, head, tail, xu_predicate_eq, value);
}
xu_size_t xu_find_all(xu_iterator_ref_t iterator, xu_cpointer_t value)
{
    return xu_find_all_if(iterator, xu_predicate_eq, value);
}
