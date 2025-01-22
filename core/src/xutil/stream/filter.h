#pragma once

#include "xutil/prefix.h"
#include "xutil/stream/impl/filter.h"

/* *******************************************************
 * macros
 */
// the filter ctrl
#define XU_FILTER_CTRL(type, ctrl) (((type) << 16) | (ctrl))

/* *******************************************************
 * types
 */
// the filter type enum
typedef enum __xu_filter_type_e
{
    XU_FILTER_TYPE_NONE    = 0,
    XU_FILTER_TYPE_ZIP     = 1,
    XU_FILTER_TYPE_CACHE   = 2,
    XU_FILTER_TYPE_CHARSET = 3,
    XU_FILTER_TYPE_CHUNKED = 4

} xu_filter_type_e;

// the filter ctrl enum
typedef enum __xu_filter_ctrl_e
{
    XU_FILTER_CTRL_NONE = 0

    ,
    XU_FILTER_CTRL_ZIP_GET_ALGO   = XU_FILTER_CTRL(XU_FILTER_TYPE_ZIP, 1),
    XU_FILTER_CTRL_ZIP_GET_ACTION = XU_FILTER_CTRL(XU_FILTER_TYPE_ZIP, 2),
    XU_FILTER_CTRL_ZIP_SET_ALGO   = XU_FILTER_CTRL(XU_FILTER_TYPE_ZIP, 3),
    XU_FILTER_CTRL_ZIP_SET_ACTION = XU_FILTER_CTRL(XU_FILTER_TYPE_ZIP, 4)

        ,
    XU_FILTER_CTRL_CHARSET_GET_FTYPE = XU_FILTER_CTRL(XU_FILTER_TYPE_CHARSET, 1),
    XU_FILTER_CTRL_CHARSET_GET_TTYPE = XU_FILTER_CTRL(XU_FILTER_TYPE_CHARSET, 2),
    XU_FILTER_CTRL_CHARSET_SET_FTYPE = XU_FILTER_CTRL(XU_FILTER_TYPE_CHARSET, 3),
    XU_FILTER_CTRL_CHARSET_SET_TTYPE = XU_FILTER_CTRL(XU_FILTER_TYPE_CHARSET, 4)

} xu_filter_ctrl_e;

// the filter ref type
typedef __xu_typeref__(filter);

/* *******************************************************
 * interfaces
 */

/*! init filter from zip
 *
 * @param algo          the zip algorithm
 * @param action        the zip action
 *
 * @return              the filter
 */
xu_filter_ref_t xu_filter_init_from_zip(xu_size_t algo, xu_size_t action);

/*! init filter from charset
 *
 * @param fr            the from charset
 * @param to            the to charset
 *
 * @return              the filter
 */
xu_filter_ref_t xu_filter_init_from_charset(xu_size_t fr, xu_size_t to);

/*! init filter from chunked
 *
 * @param dechunked decode the chunked data?
 *
 * @return              the filter
 */
xu_filter_ref_t xu_filter_init_from_chunked(xu_bool_t dechunked);

/*! init filter from cache
 *
 * @param size          the initial cache size, using the default size if be zero
 *
 * @return              the filter
 */
xu_filter_ref_t xu_filter_init_from_cache(xu_size_t size);

/*! exit filter
 *
 * @param filter        the filter
 */
xu_void_t xu_filter_exit(xu_filter_ref_t filter);

/*! open filter
 *
 * @param filter        the filter
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_filter_open(xu_filter_ref_t filter);

/*! clos filter
 *
 * @param filter        the filter
 */
xu_void_t xu_filter_clos(xu_filter_ref_t filter);

/*! ctrl filter
 *
 * @param filter        the filter
 * @param ctrl          the ctrl code
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_filter_ctrl(xu_filter_ref_t filter, xu_size_t ctrl, ...);

/*! is eof for the filter input data, but the output maybe exists the left data and need flush it
 *
 * @param filter        the filter
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_filter_beof(xu_filter_ref_t filter);

/*! limit the input size for filter
 *
 * @param filter        the filter
 * @param limit         the input limit size
 */
xu_void_t xu_filter_limit(xu_filter_ref_t filter, xu_hong_t limit);

/*! push data to the filter input data
 *
 * @param filter        the filter
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_filter_push(xu_filter_ref_t filter, xu_byte_t const* data, xu_size_t size);

/*! spak filter
 *
 * @param filter        the filter
 * @param data          the input data, maybe null
 * @param size          the input size, maybe zero
 * @param pdata         the output data
 * @param need          the need output size, using the default size if zero
 * @param sync          sync? 1: sync, 0: no sync, -1: end
 *
 * @return              > 0: the output size, 0: continue, -1: end
 */
xu_long_t xu_filter_spak(xu_filter_ref_t filter, xu_byte_t const* data, xu_size_t size, xu_byte_t const** pdata,
                         xu_size_t need, xu_long_t sync);
