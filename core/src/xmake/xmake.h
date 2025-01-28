#pragma once

/* *******************************************************
 * includes
 */
#include "xmake/engine.h"
#include "xmake/prefix.h"

/* *******************************************************
 * macros
 */
#ifdef __xm_debug__
#    define __xm_mode_debug__ XU_MODE_DEBUG
#else
#    define __xm_mode_debug__ (0)
#endif

#ifdef __xm_small__
#    define __xm_mode_small__ XU_MODE_SMALL
#else
#    define __xm_mode_small__ (0)
#endif

/*! init xmake
 *
 * @return          xu_true or xu_false
 *
 * @code
    #include "xmake/xmake.h"

    int main(int argc, char** argv)
    {
        // init xmake
        if (!xm_init()) return 0;


        // exit xmake
        xm_exit();
        return 0;
    }
 * @endcode
 */
#define xm_init() xm_init_((xu_size_t)(__xm_mode_debug__ | __xm_mode_small__), XM_VERSION_BUILD)

/* *******************************************************
 * interfaces
 */

/*! init the xmake library
 *
 * @param mode      the compile mode for check __xm_small__ and __xm_debug__
 * @param build     the build version
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xm_init_(xu_size_t mode, xu_hize_t build);

/*! exit the xmake library
 *
 */
xu_void_t xm_exit(xu_noarg_t);

/*! the xmake version
 *
 * @return          the xmake version
 */
xu_version_t const* xm_version(xu_noarg_t);
