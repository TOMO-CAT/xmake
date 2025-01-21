#include "xutil/platform/syserror.h"
#include "xutil/utils/state.h"
#include <errno.h>

xu_size_t xu_syserror_state()
{
    // get last error
    switch (errno)
    {
    case EPERM: return XU_STATE_SYSERROR_NOT_PERM;
    case ENOENT: return XU_STATE_SYSERROR_NOT_FILEDIR;
    case EACCES: return XU_STATE_SYSERROR_NOT_ACCESS;
    default: return XU_STATE_SYSERROR_UNKNOWN_ERROR;
    }
}
