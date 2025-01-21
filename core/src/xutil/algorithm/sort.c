#include "xutil/algorithm/sort.h"
#include "xutil/algorithm/distance.h"
#include "xutil/algorithm/heap_sort.h"
#include "xutil/algorithm/insert_sort.h"
#include "xutil/algorithm/quick_sort.h"

xu_void_t xu_sort(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail, xu_iterator_comp_t comp)
{
    // check
    xu_assert_and_check_return(iterator);

    // no elements?
    xu_check_return(head != tail);

    // readonly?
    xu_assert_and_check_return(!(xu_iterator_mode(iterator) & XU_ITERATOR_MODE_READONLY));

#ifdef XU_CONFIG_MICRO_ENABLE
    // random access iterator?
    xu_assert_and_check_return(xu_iterator_mode(iterator) & XU_ITERATOR_MODE_RACCESS);

    // sort it
    xu_quick_sort(iterator, head, tail, comp);
#else
    // random access iterator?
    if (xu_iterator_mode(iterator) & XU_ITERATOR_MODE_RACCESS)
    {
        if (xu_distance(iterator, head, tail) > 100000)
            xu_heap_sort(iterator, head, tail, comp);
        else
            xu_quick_sort(iterator, head, tail, comp); //!< @note the recursive stack size is limit
    }
    else
        xu_insert_sort(iterator, head, tail, comp);
#endif
}
xu_void_t xu_sort_all(xu_iterator_ref_t iterator, xu_iterator_comp_t comp)
{
    xu_sort(iterator, xu_iterator_head(iterator), xu_iterator_tail(iterator), comp);
}
