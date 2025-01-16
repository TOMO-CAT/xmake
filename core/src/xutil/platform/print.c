#include "xutil/platform/print.h"

#if defined(XU_CONFIG_OS_ANDROID)
#    include "xutil/platform/android/print.c"
#elif defined(XU_CONFIG_OS_IOS)
#    include "xutil/platform/mach/ios/print.c"
#elif defined(XU_CONFIG_LIBC_HAVE_FPUTS)
#    include "xutil/platform/libc/print.c"
#else
xu_void_t xu_print(xu_char_t const* string) { xu_trace_noimpl(); }
xu_void_t xu_printl(xu_char_t const* string) { xu_trace_noimpl(); }
xu_void_t xu_print_sync() { xu_trace_noimpl(); }
#endif
