#include "xutil/algorithm/distance.h"

xu_size_t xu_distance(xu_iterator_ref_t iterator, xu_size_t head, xu_size_t tail)
{
    // check
    xu_assert_and_check_return_val(iterator, 0);

    // zero distance?
    xu_check_return_val(head != tail, 0);

    // the iterator mode
    xu_size_t mode = xu_iterator_mode(iterator);

    // random access iterator?
    xu_size_t distance = 0;
    if (mode & XU_ITERATOR_MODE_RACCESS)
    {
        // compute it fast
        distance = tail - head;
    }
    // forward iterator?
    else if (mode & XU_ITERATOR_MODE_FORWARD)
    {
        // whole container?
        if (xu_iterator_head(iterator) == head && xu_iterator_tail(iterator) == tail)
            distance = xu_iterator_size(iterator);
        else
        {
            // done
            xu_size_t itor = head;
            for (; itor != tail; itor = xu_iterator_next(iterator, itor))
                distance++;
        }
    }
    // reverse iterator?
    else if (mode & XU_ITERATOR_MODE_REVERSE)
    {
        // whole container?
        if (xu_iterator_head(iterator) == head && xu_iterator_tail(iterator) == tail)
            distance = xu_iterator_size(iterator);
        else
        {
            // done
            xu_size_t itor = tail;
            do
            {
                // update the distance
                distance++;

                // the previous
                itor = xu_iterator_prev(iterator, itor);

            } while (itor != head);
        }
    }
    // unknown mode?
    else
    {
        // abort
        xu_assert(0);
    }

    // ok?
    return distance;
}
