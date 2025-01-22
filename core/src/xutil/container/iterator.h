#pragma once

#include "xutil/macros/assert.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 */
// the iterator mode enum
typedef enum __xu_iterator_mode_e
{
    XU_ITERATOR_MODE_FORWARD = 1 //!< forward iterator
    ,
    XU_ITERATOR_MODE_REVERSE = 2 //!< reverse iterator
    ,
    XU_ITERATOR_MODE_RACCESS = 4 //!< random access iterator
    ,
    XU_ITERATOR_MODE_MUTABLE = 8 //!< mutable iterator, the item of the same iterator is mutable for removing and
                                 //!< moving, .e.g vector, hash, ...
    ,
    XU_ITERATOR_MODE_READONLY = 16 //!< readonly iterator

} xu_iterator_mode_e;

// the iterator flag enum
typedef enum __xu_iterator_flag_e
{
    XU_ITERATOR_FLAG_ITEM_VAL = 1 //!< the value item: int, pointer, c-string
    ,
    XU_ITERATOR_FLAG_ITEM_REF = 2 //!< the reference of value, &value

} xu_iterator_flag_e;

// the iterator operation type
struct __xu_iterator_t;
typedef struct __xu_iterator_op_t
{
    /// the iterator size
    xu_size_t (*size)(struct __xu_iterator_t* iterator);

    /// the iterator head
    xu_size_t (*head)(struct __xu_iterator_t* iterator);

    /// the iterator last
    xu_size_t (*last)(struct __xu_iterator_t* iterator);

    /// the iterator tail
    xu_size_t (*tail)(struct __xu_iterator_t* iterator);

    /// the iterator prev
    xu_size_t (*prev)(struct __xu_iterator_t* iterator, xu_size_t itor);

    /// the iterator next
    xu_size_t (*next)(struct __xu_iterator_t* iterator, xu_size_t itor);

    /// the iterator item
    xu_pointer_t (*item)(struct __xu_iterator_t* iterator, xu_size_t itor);

    /// the iterator comp
    xu_long_t (*comp)(struct __xu_iterator_t* iterator, xu_cpointer_t litem, xu_cpointer_t ritem);

    /// the iterator copy
    xu_void_t (*copy)(struct __xu_iterator_t* iterator, xu_size_t itor, xu_cpointer_t item);

    /// the iterator remove
    xu_void_t (*remove)(struct __xu_iterator_t* iterator, xu_size_t itor);

    /// the iterator nremove
    xu_void_t (*nremove)(struct __xu_iterator_t* iterator, xu_size_t prev, xu_size_t next, xu_size_t size);

} xu_iterator_op_t;

// the iterator operation ref type
typedef xu_iterator_op_t const* xu_iterator_op_ref_t;

// the iterator type
typedef struct __xu_iterator_t
{
    /// the iterator mode
    xu_uint32_t mode : 8;

    /// the iterator flag
    xu_uint32_t flag : 8;

    /// the iterator step
    xu_uint32_t step : 16;

    /// the iterator priv
    xu_pointer_t priv;

    /// the iterator operation
    xu_iterator_op_ref_t op;

} xu_iterator_t, *xu_iterator_ref_t;

// the iterator comp func type
typedef xu_long_t (*xu_iterator_comp_t)(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem);

/* *******************************************************
 * interfaces
 */
/*! the iterator mode
 *
 * @param iterator  the iterator
 *
 * @return          the iterator mode
 */
xu_size_t xu_iterator_mode(xu_iterator_ref_t iterator);

/*! the iterator flag
 *
 * @param iterator  the iterator
 *
 * @return          the iterator flag
 */
xu_size_t xu_iterator_flag(xu_iterator_ref_t iterator);

/*! the iterator step
 *
 * @param iterator  the iterator
 *
 * @return          the iterator step
 */
xu_size_t xu_iterator_step(xu_iterator_ref_t iterator);

/*! the iterator size
 *
 * @param iterator  the iterator
 *
 * @return          the iterator size
 */
xu_size_t xu_iterator_size(xu_iterator_ref_t iterator);

/*! the iterator head
 *
 * @param iterator  the iterator
 *
 * @return          the iterator head
 */
xu_size_t xu_iterator_head(xu_iterator_ref_t iterator);

/*! the iterator last
 *
 * @param iterator  the iterator
 *
 * @return          the iterator last
 */
xu_size_t xu_iterator_last(xu_iterator_ref_t iterator);

/*! the iterator tail
 *
 * @param iterator  the iterator
 *
 * @return          the iterator tail
 */
xu_size_t xu_iterator_tail(xu_iterator_ref_t iterator);

/*! the iterator prev
 *
 * @param iterator  the iterator
 * @param itor      the item itor
 *
 * @return          the iterator prev
 */
xu_size_t xu_iterator_prev(xu_iterator_ref_t iterator, xu_size_t itor);

/*! the iterator next
 *
 * @param iterator  the iterator
 * @param itor      the item itor
 *
 * @return          the iterator next
 */
xu_size_t xu_iterator_next(xu_iterator_ref_t iterator, xu_size_t itor);

/*! the iterator item
 *
 * @param iterator  the iterator
 * @param itor      the item itor
 *
 * @return          the iterator item
 */
xu_pointer_t xu_iterator_item(xu_iterator_ref_t iterator, xu_size_t itor);

/*! remove the iterator item
 *
 * @param iterator  the iterator
 * @param itor      the item itor
 */
xu_void_t xu_iterator_remove(xu_iterator_ref_t iterator, xu_size_t itor);

/*! remove the iterator items from range(prev, next)
 *
 * @param iterator  the iterator
 * @param prev      the prev item
 * @param next      the next item
 * @param size      the removed size
 */
xu_void_t xu_iterator_nremove(xu_iterator_ref_t iterator, xu_size_t prev, xu_size_t next, xu_size_t size);

/*! copy the iterator item
 *
 * @param iterator  the iterator
 * @param itor      the item itor
 * @param item      the copied item
 */
xu_void_t xu_iterator_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item);

/*! compare the iterator item
 *
 * @param iterator  the iterator
 * @param litem     the item
 * @param ritem     the compared item
 *
 * @return          =: 0, >: 1, <: -1
 */
xu_long_t xu_iterator_comp(xu_iterator_ref_t iterator, xu_cpointer_t litem, xu_cpointer_t ritem);
