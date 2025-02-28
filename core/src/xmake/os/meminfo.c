/*!A cross-platform build utility based on Lua
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2015-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        meminfo.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "meminfo"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"
#if defined(XU_CONFIG_OS_MACOSX)
#    include <mach-o/dyld.h>
#    include <mach-o/nlist.h>
#    include <mach/mach.h>
#    include <mach/machine.h>
#    include <mach/vm_statistics.h>
#elif defined(XU_CONFIG_OS_LINUX)
#    include <stdio.h>
#    include <sys/sysinfo.h>
#elif defined(XU_CONFIG_OS_BSD)
#    include <sys/sysctl.h>
#    include <sys/types.h>
#endif

/* *******************************************************
 * private implementation
 */
#ifdef XU_CONFIG_OS_LINUX
static xu_int64_t xm_os_meminfo_get_value(xu_char_t const* buffer, xu_char_t const* name)
{
    xu_char_t const* p = xu_strstr(buffer, name);
    return p ? xu_stoi64(p + xu_strlen(name)) : 0;
}
#endif

// get the used memory size (MB)
static xu_bool_t xm_os_meminfo_stats(xu_int_t* ptotalsize, xu_int_t* pavailsize)
{
#if defined(XU_CONFIG_OS_MACOSX)
    vm_statistics64_data_t vmstat;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info_t)&vmstat, &count) == KERN_SUCCESS)
    {
        xu_int_t   pagesize  = (xu_int_t)xu_page_size();
        xu_int64_t totalsize = (xu_int64_t)(vmstat.inactive_count + vmstat.free_count + vmstat.active_count +
                                            vmstat.wire_count + vmstat.compressor_page_count) *
                               pagesize;
        /*
         * NB: speculative pages are already accounted for in "free_count",
         * so "speculative_count" is the number of "free" pages that are
         * used to hold data that was read speculatively from disk but
         * haven't actually been used by anyone so far.
         *
         */
        xu_int64_t availsize =
            (xu_int64_t)(vmstat.inactive_count + vmstat.free_count - vmstat.speculative_count) * pagesize;
        *ptotalsize = (xu_int_t)(totalsize / (1024 * 1024));
        *pavailsize = (xu_int_t)(availsize / (1024 * 1024));
        return xu_true;
    }
#elif defined(XU_CONFIG_OS_LINUX)
    /* we get meminfo from /proc/meminfo
     *
     * @see https://github.com/rfjakob/earlyoom/blob/cba1d599e4a7484c45ac017aa7702ff879f15846/meminfo.c#L52
     */
    if (xu_file_info("/proc/meminfo", xu_null))
    {
        xu_bool_t ok = xu_false;
        FILE*     fp = fopen("/proc/meminfo", "r");
        if (fp)
        {
            // 8192 should be enough for the foreseeable future.
            xu_char_t buffer[8192];
            size_t    len = fread(buffer, 1, sizeof(buffer) - 1, fp);
            if (!ferror(fp) && len)
            {
                xu_int64_t totalsize = xm_os_meminfo_get_value(buffer, "MemTotal:");
                xu_int64_t availsize = xm_os_meminfo_get_value(buffer, "MemAvailable:");
                if (availsize <= 0)
                {
                    xu_int64_t cachesize  = xm_os_meminfo_get_value(buffer, "Cached:");
                    xu_int64_t freesize   = xm_os_meminfo_get_value(buffer, "MemFree:");
                    xu_int64_t buffersize = xm_os_meminfo_get_value(buffer, "Buffers:");
                    xu_int64_t shmemsize  = xm_os_meminfo_get_value(buffer, "Shmem:");
                    if (cachesize >= 0 && freesize >= 0 && buffersize >= 0 && shmemsize >= 0)
                        availsize = freesize + buffersize + cachesize - shmemsize;
                }
                if (totalsize > 0 && availsize >= 0)
                {
                    *ptotalsize = (xu_int_t)(totalsize / 1024);
                    *pavailsize = (xu_int_t)(availsize / 1024);
                    ok          = xu_true;
                }
            }
            fclose(fp);
        }
        return ok;
    }
    else
    {
        struct sysinfo info = {0};
        if (sysinfo(&info) == 0)
        {
            *ptotalsize = (xu_int_t)(info.totalram / (1024 * 1024));
            *pavailsize = (xu_int_t)((info.freeram + info.bufferram /* + cache size */) / (1024 * 1024));
            return xu_true;
        }
    }
#elif defined(XU_CONFIG_OS_BSD) && !defined(__OpenBSD__)
    unsigned long totalsize;
    size_t        size = sizeof(totalsize);
    if (sysctlbyname("hw.physmem", &totalsize, &size, xu_null, 0) != 0) return xu_false;

    // http://web.mit.edu/freebsd/head/usr.bin/systat/vmstat.c
    xu_uint32_t v_free_count;
    size = sizeof(v_free_count);
    if (sysctlbyname("vm.stats.vm.v_free_count", &v_free_count, &size, xu_null, 0) != 0) return xu_false;

    xu_uint32_t v_inactive_count;
    size = sizeof(v_inactive_count);
    if (sysctlbyname("vm.stats.vm.v_inactive_count", &v_inactive_count, &size, xu_null, 0) != 0) return xu_false;

    *ptotalsize = (xu_int_t)(totalsize / (1024 * 1024));
    *pavailsize = (xu_int_t)(((xu_int64_t)(v_free_count + v_inactive_count) * xu_page_size()) / (1024 * 1024));
    return xu_true;
#endif
    return xu_false;
}

/* *******************************************************
 * implementation
 */
xu_int_t xm_os_meminfo(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // init table
    lua_newtable(lua);

    // get the pagesize (bytes)
    xu_int_t pagesize = (xu_int_t)xu_page_size();
    lua_pushstring(lua, "pagesize");
    lua_pushinteger(lua, pagesize);
    lua_settable(lua, -3);

    // get the memory size (MB)
    xu_int_t availsize = 0;
    xu_int_t totalsize = 0;
    if (xm_os_meminfo_stats(&totalsize, &availsize))
    {
        lua_pushstring(lua, "totalsize");
        lua_pushinteger(lua, totalsize);
        lua_settable(lua, -3);

        lua_pushstring(lua, "availsize");
        lua_pushinteger(lua, availsize);
        lua_settable(lua, -3);
    }

    return 1;
}
