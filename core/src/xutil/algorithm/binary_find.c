#include "xutil/algorithm/binary_find.h"

xu_size_t xu_binary_find(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_cpointer_t item)
{
    return xu_binary_find_if(iterator, head, tail, xu_iterator_comp, item);
}
xu_size_t xu_binary_find_all(xu_iterator_ref_t iterator, xu_cpointer_t item)
{
    return xu_binary_find_all_if(iterator, xu_iterator_comp, item);
}
