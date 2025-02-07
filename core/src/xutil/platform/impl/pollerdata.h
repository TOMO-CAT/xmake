
#pragma once

/* *******************************************************
 * includes
 */
#include "xutil/platform/poller.h"

/* *******************************************************
 * macros
 */

// fd to pointer
#define xu_fd2ptr(fd) ((fd) >= 0 ? (xu_pointer_t)((xu_long_t)(fd) + 1) : xu_null)

// pointer to fd
#define xu_ptr2fd(ptr) (xu_int_t)((ptr) ? (((xu_long_t)(ptr)) - 1) : -1)

/* *******************************************************
 * types
 */

// the poller data type
typedef struct __xu_pollerdata_t
{
    // the poller data (fd => priv)
    xu_cpointer_t* data;

    // the poller data maximum count
    xu_size_t maxn;

} xu_pollerdata_t, *xu_pollerdata_ref_t;

/* *******************************************************
 * interfaces
 */

/* init poller data
 *
 * @param pollerdata    the pollerdata
 */
xu_void_t xu_pollerdata_init(xu_pollerdata_ref_t pollerdata);

/* exit poller data
 *
 * @param pollerdata    the pollerdata
 */
xu_void_t xu_pollerdata_exit(xu_pollerdata_ref_t pollerdata);

/* clear poller data
 *
 * @param pollerdata    the pollerdata
 */
xu_void_t xu_pollerdata_clear(xu_pollerdata_ref_t pollerdata);

/* set poller data
 *
 * @param pollerdata    the pollerdata
 * @param object        the poller object
 * @param priv          the poller private data
 */
xu_void_t xu_pollerdata_set(xu_pollerdata_ref_t pollerdata, xu_poller_object_ref_t object, xu_cpointer_t priv);

/* get poller data
 *
 * @param pollerdata    the pollerdata
 * @param object        the poller object
 *
 * @return              the poller private data
 */
xu_cpointer_t xu_pollerdata_get(xu_pollerdata_ref_t pollerdata, xu_poller_object_ref_t object);

/* reset poller data
 *
 * @param pollerdata    the pollerdata
 * @param object        the poller object
 */
xu_void_t xu_pollerdata_reset(xu_pollerdata_ref_t pollerdata, xu_poller_object_ref_t object);
