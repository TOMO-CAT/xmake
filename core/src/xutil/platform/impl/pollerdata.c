/* *******************************************************
 * includes
 */
#include "xutil/platform/impl/pollerdata.h"
#include "xutil/libc/libc.h"
#include "xutil/platform/pipe.h"
#include "xutil/platform/thread_local.h"

/* *******************************************************
 * macros
 */
#ifdef __xu_small__
#    define XU_POLLERDATA_GROW (64)
#else
#    define XU_POLLERDATA_GROW (256)
#endif

/* *******************************************************
 * private implementation
 */
static xu_long_t xu_pollerdata_object2fd(xu_poller_object_ref_t object)
{
    // check
    xu_assert(object && object->ref.ptr);

    // get the fd
    xu_long_t fd;
    fd = xu_ptr2fd(object->ref.ptr);
    xu_assert(fd > 0 && fd < XU_MAXS16);

    return fd;
}

/* *******************************************************
 * implementation
 */

xu_void_t xu_pollerdata_init(xu_pollerdata_ref_t pollerdata)
{
    // check
    xu_assert(pollerdata);

    // init it
    pollerdata->data = xu_null;
    pollerdata->maxn = 0;
}
xu_void_t xu_pollerdata_exit(xu_pollerdata_ref_t pollerdata)
{
    // check
    xu_assert(pollerdata);

    // exit poller data
    if (pollerdata->data) xu_free(pollerdata->data);
    pollerdata->data = xu_null;
    pollerdata->maxn = 0;
}
xu_void_t xu_pollerdata_clear(xu_pollerdata_ref_t pollerdata)
{
    // check
    xu_assert(pollerdata);

    // clear data
    if (pollerdata->data) xu_memset(pollerdata->data, 0, pollerdata->maxn * sizeof(xu_cpointer_t));
}
xu_cpointer_t xu_pollerdata_get(xu_pollerdata_ref_t pollerdata, xu_poller_object_ref_t object)
{
    // check
    xu_assert(pollerdata);

    // get the poller private data
    xu_long_t fd = xu_pollerdata_object2fd(object);
    return (pollerdata->data && fd < pollerdata->maxn) ? pollerdata->data[fd] : xu_null;
}
xu_void_t xu_pollerdata_set(xu_pollerdata_ref_t pollerdata, xu_poller_object_ref_t object, xu_cpointer_t priv)
{
    // check
    xu_assert(pollerdata && object);

    // get fd
    xu_long_t fd = xu_pollerdata_object2fd(object);

    // no data? init it first
    xu_size_t need = fd + 1;
    if (!pollerdata->data)
    {
        // init data
        need += XU_POLLERDATA_GROW;
        pollerdata->data = xu_nalloc0_type(need, xu_cpointer_t);
        xu_assert_and_check_return(pollerdata->data);

        // init data size
        pollerdata->maxn = need;
    }
    else if (need > pollerdata->maxn)
    {
        // grow data
        need += XU_POLLERDATA_GROW;
        pollerdata->data = (xu_cpointer_t*)xu_ralloc(pollerdata->data, need * sizeof(xu_cpointer_t));
        xu_assert_and_check_return(pollerdata->data);

        // init growed space
        xu_memset(pollerdata->data + pollerdata->maxn, 0, (need - pollerdata->maxn) * sizeof(xu_cpointer_t));

        // grow data size
        pollerdata->maxn = need;
    }

    // save the poller private data
    pollerdata->data[fd] = priv;
}
xu_void_t xu_pollerdata_reset(xu_pollerdata_ref_t pollerdata, xu_poller_object_ref_t object)
{
    // check
    xu_assert(pollerdata && object);

    // get fd
    xu_long_t fd = xu_pollerdata_object2fd(object);

    // remove the poller private data
    if (fd < pollerdata->maxn) pollerdata->data[fd] = xu_null;
}
