#include "xutil/platform/page.h"
#include "xutil/prefix.h"
#include <unistd.h>

// the page size
static xu_size_t g_page_size = 0;

xu_bool_t xu_page_init()
{
    // init page size
    if (!g_page_size)
    {
#if defined(XU_CONFIG_POSIX_HAVE_SYSCONF) && defined(_SC_PAGESIZE)
        g_page_size = (xu_size_t)sysconf(_SC_PAGESIZE);
#elif defined(XU_CONFIG_POSIX_HAVE_GETPAGESIZE)
        g_page_size = (xu_size_t)getpagesize();
#endif
    }

    // ok?
    return g_page_size ? xu_true : xu_false;
}
xu_void_t xu_page_exit() {}
xu_size_t xu_page_size() { return g_page_size; }
