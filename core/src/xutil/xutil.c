#include "xutil/xutil.h"
#include "xutil/libc/impl/libc.h"
#include "xutil/libm/impl/libm.h"
#include "xutil/math/impl/math.h"
#include "xutil/network/impl/network.h"
#include "xutil/platform/impl/platform.h"
#include "xutil/utils/singleton.h"

/* *******************************************************
 *  globals
 * *******************************************************
 */

// the state
static xu_atomic32_t g_state = XU_STATE_END;

/* *******************************************************
 *  private implementation
 * *******************************************************
 */
static __xu_inline__ xu_bool_t xu_check_order_word()
{
    xu_uint16_t      x = 0x1234;
    xu_byte_t const* p = (xu_byte_t const*)&x;

#ifdef XU_WORDS_BIGENDIAN
    // is big endian?
    return (p[0] == 0x12 && p[1] == 0x34) ? xu_true : xu_false;
#else
    // is little endian?
    return (p[0] == 0x34 && p[1] == 0x12) ? xu_true : xu_false;
#endif
}
static __xu_inline__ xu_bool_t xu_check_order_double()
{
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
    union
    {
        xu_uint32_t i[2];
        double      f;

    } conv;
    conv.f = 1.0f;

#    ifdef XU_FLOAT_BIGENDIAN
    // is big endian?
    return (!conv.i[1] && conv.i[0]) ? xu_true : xu_false;
#    else
    // is little endian?
    return (!conv.i[0] && conv.i[1]) ? xu_true : xu_false;
#    endif
#else
    return xu_true;
#endif
}
static __xu_inline__ xu_bool_t xu_check_mode(xu_size_t mode)
{
#ifdef __xu_debug__
    if (!(mode & XU_MODE_DEBUG))
    {
        xu_trace_e("libxutil.a has __xu_debug__ but xutil/xutil.h not");
        return xu_false;
    }
#else
    if (mode & XU_MODE_DEBUG)
    {
        xu_trace_e("xutil/xutil.h has __xu_debug__ but libxutil.a not");
        return xu_false;
    }
#endif

#ifdef __xu_small__
    if (!(mode & XU_MODE_SMALL))
    {
        xu_trace_e("libxutil.a has __xu_small__ but xutil/xutil.h not");
        return xu_false;
    }
#else
    if (mode & XU_MODE_SMALL)
    {
        xu_trace_e("xutil/xutil.h has __xu_small__ but libxutil.a not");
        return xu_false;
    }
#endif

    // ok
    return xu_true;
}
static __xu_inline__ xu_bool_t xu_version_check(xu_hize_t build)
{
#ifdef XU_CONFIG_INFO_HAVE_VERSION
    // the version oly for link the static vtag string
    xu_version_t const* version = xu_version();
    xu_used(version);
#endif

    // ok
    if ((build / 100) == (XU_VERSION_BUILD / 100))
    {
        xu_trace_d("version: %s", XU_VERSION_STRING);
        return xu_true;
    }
    else
    {
        xu_trace_w("version: %s != %llu", XU_VERSION_STRING, build);
    }

    // no
    return xu_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
xu_bool_t xutil_init_(xu_handle_t priv, xu_allocator_ref_t allocator, xu_size_t mode, xu_hize_t build)
{
    // have been initialized?
    if (XU_STATE_OK == xu_atomic32_fetch_and_cmpset(&g_state, XU_STATE_END, XU_STATE_OK)) return xu_true;

    // init trace
    if (!xu_trace_init()) return xu_false;

    // trace
    xu_trace_d("init: ..");

    // check mode
    if (!xu_check_mode(mode)) return xu_false;

    // check types
    xu_assert_static(sizeof(xu_byte_t) == 1);
    xu_assert_static(sizeof(xu_uint_t) == 4);
    xu_assert_static(sizeof(xu_uint8_t) == 1);
    xu_assert_static(sizeof(xu_uint16_t) == 2);
    xu_assert_static(sizeof(xu_uint32_t) == 4);
    xu_assert_static(sizeof(xu_hize_t) == 8);
    xu_assert_static(sizeof(xu_wchar_t) == sizeof(L'w'));
    xu_assert_static(XU_CPU_BITSIZE == (sizeof(xu_size_t) << 3));
    xu_assert_static(XU_CPU_BITSIZE == (sizeof(xu_long_t) << 3));
    xu_assert_static(XU_CPU_BITSIZE == (sizeof(xu_pointer_t) << 3));
    xu_assert_static(XU_CPU_BITSIZE == (sizeof(xu_handle_t) << 3));

    // check byteorder
    xu_assert_and_check_return_val(xu_check_order_word(), xu_false);
    xu_assert_and_check_return_val(xu_check_order_double(), xu_false);

    // init singleton
    if (!xu_singleton_init()) return xu_false;

    // init memory environment
    if (!xu_memory_init_env(allocator)) return xu_false;

    // init platform environment
    if (!xu_platform_init_env(priv)) return xu_false;

    // init libc environment
    if (!xu_libc_init_env()) return xu_false;

    // init math environment
    if (!xu_math_init_env()) return xu_false;

    // init libm environment
    if (!xu_libm_init_env()) return xu_false;

    // init network environment
    if (!xu_network_init_env()) return xu_false;

    // check version
    xu_version_check(build);

    // trace
    xu_trace_d("init: ok");

    // ok
    return xu_true;
}
xu_void_t xutil_exit()
{
    // have been exited?
    if (XU_STATE_OK != xu_atomic32_fetch_and_cmpset(&g_state, XU_STATE_OK, XU_STATE_EXITING)) return;

    // kill singleton
    xu_singleton_kill();

    // exit network environment
    xu_network_exit_env();

    // exit libm environment
    xu_libm_exit_env();

    // exit math environment
    xu_math_exit_env();

    // exit libc environment
    xu_libc_exit_env();

    // exit platform environment
    xu_platform_exit_env();

    // exit singleton
    xu_singleton_exit();

    // exit memory environment
    xu_memory_exit_env();

    // trace
    xu_trace_d("exit: ok");

    // exit trace
    xu_trace_exit();

    // end
    xu_atomic32_set(&g_state, XU_STATE_END);
}
xu_size_t xutil_state()
{
    // get state
    return (xu_size_t)xu_atomic32_get(&g_state);
}
#ifdef XU_CONFIG_INFO_HAVE_VERSION
xu_version_t const* xu_version()
{
    // init version tag for binary search
    static __xu_volatile__ xu_char_t const* s_vtag = "[xutil]: [vtag]: " XU_VERSION_STRING;
    xu_used(s_vtag);

    // init version
    static xu_version_t s_version = {0};
    if (!s_version.major)
    {
        s_version.major = XU_VERSION_MAJOR;
        s_version.minor = XU_VERSION_MINOR;
        s_version.alter = XU_VERSION_ALTER;
        s_version.build = (xu_hize_t)xu_atoll(XU_VERSION_BUILD_STRING);
    }

    return &s_version;
}
#endif
