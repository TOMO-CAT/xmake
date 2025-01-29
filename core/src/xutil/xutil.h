#pragma once

// interface headers
#include "xutil/charset/chartset.h"
#include "xutil/hash/sha.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/type.h"
#include "xutil/memory/allocator.h"
#include "xutil/memory/memory.h"
#include "xutil/platform/atomic.h"
#include "xutil/platform/directory.h"
#include "xutil/platform/environment.h"
#include "xutil/platform/file.h"
#include "xutil/platform/path.h"
#include "xutil/platform/socket.h"
#include "xutil/utils/state.h"
#include "xutil/utils/trace.h"
#include "xutil/utils/used.h"

/* *******************************************************
 * macros
 */

// the compile mode
#define XU_MODE_DEBUG (1)
#define XU_MODE_SMALL (2)

#ifdef __xu_debug__
#    define __xu_mode_debug__ XU_MODE_DEBUG
#else
#    define __xu_mode_debug__ (0)
#endif

#ifdef __xu_small__
#    define __xu_mode_small__ XU_MODE_SMALL
#else
#    define __xu_mode_small__ (0)
#endif

/*! init xutil
 *
 * @param priv      the platform private data
 *                  pass JavaVM* jvm for android jni
 *                  pass xu_null for other platform
 *
 * @param allocator the allocator, supports:
 *
 *                  - xu_native_allocator()
 *                      uses native memory directly
 *
 *                  - xu_static_allocator(data, size)
 *                      uses the a static small buffer and we can check memory error and leaking
 *
 *                  - xu_default_allocator(data, size)
 *                      uses the a large pool with the static memory and we can check memory error and leaking
 *
 *                  - xu_default_allocator(xu_null, 0)
 *                      uses the a large pool with the native memory and we can check memory error and leaking
 *
 *                  - xu_null
 *                      uses xu_default_allocator(xu_null, 0) for large mode
 *                      uses xu_native_allocator() for small mode, need define __xu_small__
 *
 * @return          xu_true or xu_false
 *
 * @code
 *
    #include "xutil/xutil.h"

    int main(int argc, char** argv)
    {
        // init xutil
        if (!xu_init(xu_null, xu_null)) return 0;

        // print info with tag
        xu_trace_i("hello xutil");

        // print info only for debug
        xu_trace_d("hello xutil");

        // print error info
        xu_trace_e("hello xutil");

        // init stream
        xu_stream_ref_t stream = xu_stream_init_from_url("http://www.xxxx.com/index.html");
        if (stream)
        {
            // save stream data to file
            xu_transfer_to_url(stream, "/home/file/index.html", 0, xu_null, xu_null);

            // exit stream
            xu_stream_exit(stream);
        }

        // ...

        // exit xutil
        xu_exit();
        return 0;
    }
 * @endcode
 */
#define xutil_init(priv, allocator)                                                                                    \
    xutil_init_(priv, allocator, (xu_size_t)(__xu_mode_debug__ | __xu_mode_small__), XU_VERSION_BUILD)

/* *******************************************************
 *  interfaces
 */

/*! init the xutil library
 *
 * @param priv      the platform private data
 *                  pass JavaVM* jvm for android jni
 *                  pass xu_null for other platform
 *
 * @param allocator the allocator, supports:
 *
 *                  - xu_native_allocator()
 *                      uses native memory directly
 *
 *                  - xu_static_allocator(data, size)
 *                      uses the a static small buffer and we can check memory error and leaking
 *
 *                  - xu_default_allocator(data, size)
 *                      uses the a large pool with the static memory and we can check memory error and leaking
 *
 *                  - xu_default_allocator(xu_null, 0)
 *                      uses the a large pool with the native memory and we can check memory error and leaking
 *
 *                  - xu_null
 *                      uses xu_default_allocator(xu_null, 0) for large mode
 *                      uses xu_native_allocator() for small mode, need define __xu_small__
 *
 * @param mode      the compile mode for check __xu_small__ and __xu_debug__
 * @param build     the build version
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xutil_init_(xu_handle_t priv, xu_allocator_ref_t allocator, xu_size_t mode, xu_hize_t build);

// exit the xutil library
xu_void_t xutil_exit(xu_noarg_t);

/*! the state
 *
 * - XU_STATE_OK
 * - XU_STATE_END
 * - XU_STATE_EXITING
 *
 * @return          the xutil state
 */
xu_size_t xutil_state(xu_noarg_t);

#ifdef XU_CONFIG_INFO_HAVE_VERSION
/*! the xutil version
 *
 * @return          the xutil version
 */
xu_version_t const* xutil_version(xu_noarg_t);
#endif
