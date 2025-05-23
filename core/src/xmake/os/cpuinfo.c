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
 * @file        cpuinfo.c
 *
 */

/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "cpuinfo"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/os/prefix.h"
#if defined(XU_CONFIG_OS_MACOSX)
#    include <mach/mach.h>
#    include <mach/mach_host.h>
#    include <mach/processor_info.h>
#    include <sys/sysctl.h>
#    include <sys/types.h>
#elif defined(XU_CONFIG_OS_LINUX)
#    include <stdio.h>
#elif defined(XU_CONFIG_OS_BSD)
#    include <stdio.h>
#    include <string.h>
#    include <sys/sysctl.h>
#    include <sys/types.h>
#    include <unistd.h>
#endif

/* *******************************************************
 * private implementation
 */

static xu_float_t xm_os_cpuinfo_usagerate()
{
#if defined(XU_CONFIG_OS_MACOSX)
    xu_float_t             usagerate = 0;
    natural_t              cpu_count = 0;
    processor_info_array_t cpuinfo;
    mach_msg_type_number_t cpuinfo_count;
    static xu_hong_t       s_time = 0;
    if (xu_mclock() - s_time > 1000 && host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &cpu_count,
                                                           &cpuinfo, &cpuinfo_count) == KERN_SUCCESS)
    {
        static processor_info_array_t s_cpuinfo_prev       = xu_null;
        static mach_msg_type_number_t s_cpuinfo_count_prev = 0;
        for (xu_int_t i = 0; i < cpu_count; ++i)
        {
            xu_int_t use, total;
            if (s_cpuinfo_prev)
            {
                use = (cpuinfo[(CPU_STATE_MAX * i) + CPU_STATE_USER] -
                       s_cpuinfo_prev[(CPU_STATE_MAX * i) + CPU_STATE_USER]) +
                      (cpuinfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM] -
                       s_cpuinfo_prev[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM]) +
                      (cpuinfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE] -
                       s_cpuinfo_prev[(CPU_STATE_MAX * i) + CPU_STATE_NICE]);
                total = use + (cpuinfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE] -
                               s_cpuinfo_prev[(CPU_STATE_MAX * i) + CPU_STATE_IDLE]);
            }
            else
            {
                use = cpuinfo[(CPU_STATE_MAX * i) + CPU_STATE_USER] + cpuinfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM] +
                      cpuinfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE];
                total = use + cpuinfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE];
            }
            usagerate += total > 0 ? ((xu_float_t)use / (xu_float_t)total) : 0;
        }
        if (s_cpuinfo_prev)
            vm_deallocate(mach_task_self(), (vm_address_t)s_cpuinfo_prev, sizeof(integer_t) * s_cpuinfo_count_prev);
        s_time               = xu_mclock();
        s_cpuinfo_prev       = cpuinfo;
        s_cpuinfo_count_prev = cpuinfo_count;
    }
    return cpu_count > 0 ? usagerate / cpu_count : 0;
#elif defined(XU_CONFIG_OS_LINUX)
    xu_float_t usagerate = 0;
    if (xu_file_info("/proc/stat", xu_null))
    {
        FILE* fp = fopen("/proc/stat", "r");
        if (fp)
        {
            xu_char_t         line[8192];
            static xu_int64_t total_prev  = 0;
            static xu_int64_t active_prev = 0;
            while (!feof(fp))
            {
                /* cpu  548760 0 867417 102226682 12430 0 9089 0 0 0
                 * cpu0 136863 0 218110 25632388 2706 0 2328 0 0 0
                 * cpu1 148383 0 213941 25627686 3925 0 2129 0 0 0
                 *
                 * The meanings of the columns are as follows, from left to right:
                 *
                 * user: normal processes executing in user mode
                 * nice: niced processes executing in user mode
                 * system: processes executing in kernel mode
                 * idle: twiddling thumbs
                 * iowait: waiting for I/O to complete
                 * irq: servicing interrupts
                 * softirq: servicing softirqs
                 * steal
                 * guest
                 * guest_nice
                 */
                if (fgets(line, sizeof(line), fp) && !xu_strncmp(line, "cpu ", 4))
                {
                    long long user, nice, sys, idle, iowait, irq, softirq, steal, guest, guest_nice;
                    if (10 == sscanf(line, "cpu  %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", &user, &nice, &sys,
                                     &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice))
                    {
                        xu_int64_t active =
                            (xu_int64_t)(user + nice + sys + irq + softirq + steal + guest + guest_nice);
                        xu_int64_t total = (xu_int64_t)(user + nice + sys + idle + iowait + irq + softirq + steal +
                                                        guest + guest_nice);
                        if (total_prev > 0 && active_prev > 0)
                        {
                            xu_int64_t total_diff  = total - total_prev;
                            xu_int64_t active_diff = active - active_prev;
                            if (total_diff > 0) usagerate = (xu_float_t)((xu_double_t)active_diff / total_diff);
                        }
                        total_prev  = total;
                        active_prev = active;
                    }
                    break;
                }
            }
            fclose(fp);
        }
    }
    return usagerate;
#elif defined(XU_CONFIG_OS_BSD) && !defined(__OpenBSD__)
#    define CP_USER 0
#    define CP_NICE 1
#    define CP_SYS 2
#    define CP_INTR 3
#    define CP_IDLE 4
#    define CPUSTATES 5

    static xu_int64_t total_prev  = 0;
    static xu_int64_t active_prev = 0;

    xu_float_t usagerate         = 0;
    long       states[CPUSTATES] = {0};
    size_t     states_size       = sizeof(states);
    if (sysctlbyname("kern.cp_time", &states, &states_size, xu_null, 0) == 0)
    {
        xu_long_t user = states[CP_USER];
        xu_long_t nice = states[CP_NICE];
        xu_long_t sys  = states[CP_SYS];
        xu_long_t intr = states[CP_INTR];
        xu_long_t idle = states[CP_IDLE];

        xu_int64_t active = user + nice + sys + intr;
        xu_int64_t total  = user + nice + sys + idle + intr;
        if (total_prev > 0 && active_prev > 0)
        {
            xu_int64_t total_diff  = total - total_prev;
            xu_int64_t active_diff = active - active_prev;
            if (total_diff > 0) usagerate = (xu_float_t)((xu_double_t)active_diff / total_diff);
        }
        total_prev  = total;
        active_prev = active;
    }
    return usagerate;
#else
    return 0;
#endif
}

/* *******************************************************
 * implementation
 */

/* local cpuinfo = os.cpuinfo()
 * {
 *      ncpu = 4,
 *      ...
 * }
 */
xu_int_t xm_os_cpuinfo(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // init table
    lua_newtable(lua);

    // get cpu number
    xu_int_t ncpu = (xu_int_t)xu_cpu_count();
    lua_pushstring(lua, "ncpu");
    lua_pushinteger(lua, ncpu > 0 ? ncpu : 1);
    lua_settable(lua, -3);

    // get cpu usage rate
    xu_float_t usagerate = xm_os_cpuinfo_usagerate();
    if (usagerate >= 0)
    {
        lua_pushstring(lua, "usagerate");
        lua_pushnumber(lua, usagerate);
        lua_settable(lua, -3);
    }
    return 1;
}
