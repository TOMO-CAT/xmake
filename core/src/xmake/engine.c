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
 * @file        engine.c
 *
 */

/* *******************************************************
 * trace
 */

#define XU_TRACE_MODULE_NAME "engine"
#define XU_TRACE_MODULE_DEBUG (1)

/* *******************************************************
 * includes
 */

#include "xmake/xmake.h"
#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)
#    include <mach-o/dyld.h>
#    include <signal.h>
#    include <unistd.h>
#elif defined(XU_CONFIG_OS_LINUX) || defined(XU_CONFIG_OS_BSD) || defined(XU_CONFIG_OS_ANDROID) ||                     \
    defined(XU_CONFIG_OS_HAIKU)
#    include <signal.h>
#    include <unistd.h>
#endif
#ifdef XU_CONFIG_OS_BSD
#    include <signal.h>
#    include <sys/sysctl.h>
#    include <sys/types.h>
#endif
#ifdef XU_CONFIG_OS_HAIKU
#    include <image.h>
#endif

/* *******************************************************
 * macros
 */

// proc/self
#if defined(XU_CONFIG_OS_LINUX)
#    define XM_PROC_SELF_FILE "/proc/self/exe"
#elif defined(XU_CONFIG_OS_BSD) && !defined(__OpenBSD__) && !defined(__FreeBSD__)
#    if defined(__NetBSD__)
#        define XM_PROC_SELF_FILE "/proc/curproc/exe"
#    else
#        define XM_PROC_SELF_FILE "/proc/curproc/file"
#    endif
#endif

// hook lua memory allocator
#define XM_HOOK_LUA_MEMALLOC (0)

/* *******************************************************
 * types
 */

// the engine type
typedef struct __xm_engine_t
{
    // the lua
    lua_State* lua;

    // the engine name
    xu_char_t name[64];

} xm_engine_t;

/* *******************************************************
 * declaration
 */

// the os functions
xu_int_t xm_os_argv(lua_State* lua);
xu_int_t xm_os_args(lua_State* lua);
xu_int_t xm_os_find(lua_State* lua);
xu_int_t xm_os_link(lua_State* lua);
xu_int_t xm_os_isdir(lua_State* lua);
xu_int_t xm_os_rmdir(lua_State* lua);
xu_int_t xm_os_mkdir(lua_State* lua);
xu_int_t xm_os_cpdir(lua_State* lua);
xu_int_t xm_os_chdir(lua_State* lua);
xu_int_t xm_os_mtime(lua_State* lua);
xu_int_t xm_os_sleep(lua_State* lua);
xu_int_t xm_os_mclock(lua_State* lua);
xu_int_t xm_os_curdir(lua_State* lua);
xu_int_t xm_os_tmpdir(lua_State* lua);
xu_int_t xm_os_islink(lua_State* lua);
xu_int_t xm_os_isfile(lua_State* lua);
xu_int_t xm_os_touch(lua_State* lua);
xu_int_t xm_os_rmfile(lua_State* lua);
xu_int_t xm_os_cpfile(lua_State* lua);
xu_int_t xm_os_fscase(lua_State* lua);
xu_int_t xm_os_rename(lua_State* lua);
xu_int_t xm_os_exists(lua_State* lua);
xu_int_t xm_os_setenv(lua_State* lua);
xu_int_t xm_os_getenv(lua_State* lua);
xu_int_t xm_os_getenvs(lua_State* lua);
xu_int_t xm_os_cpuinfo(lua_State* lua);
xu_int_t xm_os_meminfo(lua_State* lua);
xu_int_t xm_os_readlink(lua_State* lua);
xu_int_t xm_os_filesize(lua_State* lua);
xu_int_t xm_os_emptydir(lua_State* lua);
xu_int_t xm_os_syserror(lua_State* lua);
xu_int_t xm_os_strerror(lua_State* lua);
xu_int_t xm_os_getwinsize(lua_State* lua);
xu_int_t xm_os_getpid(lua_State* lua);
xu_int_t xm_os_signal(lua_State* lua);
xu_int_t xm_os_uid(lua_State* lua);
xu_int_t xm_os_gid(lua_State* lua);
xu_int_t xm_os_getown(lua_State* lua);

// the io/file functions
xu_int_t xm_io_stdfile(lua_State* lua);
xu_int_t xm_io_file_open(lua_State* lua);
xu_int_t xm_io_file_read(lua_State* lua);
xu_int_t xm_io_file_readable(lua_State* lua);
xu_int_t xm_io_file_seek(lua_State* lua);
xu_int_t xm_io_file_size(lua_State* lua);
xu_int_t xm_io_file_rawfd(lua_State* lua);
xu_int_t xm_io_file_write(lua_State* lua);
xu_int_t xm_io_file_flush(lua_State* lua);
xu_int_t xm_io_file_close(lua_State* lua);
xu_int_t xm_io_file_isatty(lua_State* lua);

// the io/filelock functions
xu_int_t xm_io_filelock_open(lua_State* lua);
xu_int_t xm_io_filelock_lock(lua_State* lua);
xu_int_t xm_io_filelock_unlock(lua_State* lua);
xu_int_t xm_io_filelock_trylock(lua_State* lua);
xu_int_t xm_io_filelock_close(lua_State* lua);

// the io/socket functions
xu_int_t xm_io_socket_open(lua_State* lua);
xu_int_t xm_io_socket_rawfd(lua_State* lua);
xu_int_t xm_io_socket_peeraddr(lua_State* lua);
xu_int_t xm_io_socket_wait(lua_State* lua);
xu_int_t xm_io_socket_bind(lua_State* lua);
xu_int_t xm_io_socket_ctrl(lua_State* lua);
xu_int_t xm_io_socket_listen(lua_State* lua);
xu_int_t xm_io_socket_accept(lua_State* lua);
xu_int_t xm_io_socket_connect(lua_State* lua);
xu_int_t xm_io_socket_send(lua_State* lua);
xu_int_t xm_io_socket_sendto(lua_State* lua);
xu_int_t xm_io_socket_sendfile(lua_State* lua);
xu_int_t xm_io_socket_recv(lua_State* lua);
xu_int_t xm_io_socket_recvfrom(lua_State* lua);
xu_int_t xm_io_socket_kill(lua_State* lua);
xu_int_t xm_io_socket_close(lua_State* lua);

// the io/pipe functions
xu_int_t xm_io_pipe_open(lua_State* lua);
xu_int_t xm_io_pipe_openpair(lua_State* lua);
xu_int_t xm_io_pipe_close(lua_State* lua);
xu_int_t xm_io_pipe_read(lua_State* lua);
xu_int_t xm_io_pipe_write(lua_State* lua);
xu_int_t xm_io_pipe_wait(lua_State* lua);
xu_int_t xm_io_pipe_connect(lua_State* lua);

// the io/poller functions
xu_int_t xm_io_poller_insert(lua_State* lua);
xu_int_t xm_io_poller_modify(lua_State* lua);
xu_int_t xm_io_poller_remove(lua_State* lua);
xu_int_t xm_io_poller_spank(lua_State* lua);
xu_int_t xm_io_poller_support(lua_State* lua);
xu_int_t xm_io_poller_wait(lua_State* lua);

// the path functions
xu_int_t xm_path_relative(lua_State* lua);
xu_int_t xm_path_absolute(lua_State* lua);
xu_int_t xm_path_translate(lua_State* lua);
xu_int_t xm_path_directory(lua_State* lua);
xu_int_t xm_path_is_absolute(lua_State* lua);

// the hash functions
xu_int_t xm_hash_uuid4(lua_State* lua);
xu_int_t xm_hash_sha(lua_State* lua);
xu_int_t xm_hash_md5(lua_State* lua);
xu_int_t xm_hash_xxhash(lua_State* lua);

// the base64 functions
xu_int_t xm_base64_encode(lua_State* lua);
xu_int_t xm_base64_decode(lua_State* lua);

// the lz4 functions
xu_int_t xm_lz4_compress(lua_State* lua);
xu_int_t xm_lz4_decompress(lua_State* lua);
xu_int_t xm_lz4_block_compress(lua_State* lua);
xu_int_t xm_lz4_block_decompress(lua_State* lua);
xu_int_t xm_lz4_compress_file(lua_State* lua);
xu_int_t xm_lz4_decompress_file(lua_State* lua);
xu_int_t xm_lz4_compress_stream_open(lua_State* lua);
xu_int_t xm_lz4_compress_stream_read(lua_State* lua);
xu_int_t xm_lz4_compress_stream_write(lua_State* lua);
xu_int_t xm_lz4_compress_stream_close(lua_State* lua);
xu_int_t xm_lz4_decompress_stream_open(lua_State* lua);
xu_int_t xm_lz4_decompress_stream_read(lua_State* lua);
xu_int_t xm_lz4_decompress_stream_write(lua_State* lua);
xu_int_t xm_lz4_decompress_stream_close(lua_State* lua);

// the bloom filter functions
xu_int_t xm_bloom_filter_open(lua_State* lua);
xu_int_t xm_bloom_filter_close(lua_State* lua);
xu_int_t xm_bloom_filter_clear(lua_State* lua);
xu_int_t xm_bloom_filter_data(lua_State* lua);
xu_int_t xm_bloom_filter_size(lua_State* lua);
xu_int_t xm_bloom_filter_get(lua_State* lua);
xu_int_t xm_bloom_filter_set(lua_State* lua);
xu_int_t xm_bloom_filter_data_set(lua_State* lua);

// the string functions
xu_int_t xm_string_trim(lua_State* lua);
xu_int_t xm_string_split(lua_State* lua);
xu_int_t xm_string_lastof(lua_State* lua);
xu_int_t xm_string_convert(lua_State* lua);
xu_int_t xm_string_endswith(lua_State* lua);
xu_int_t xm_string_startswith(lua_State* lua);

// the process functions
xu_int_t xm_process_open(lua_State* lua);
xu_int_t xm_process_openv(lua_State* lua);
xu_int_t xm_process_wait(lua_State* lua);
xu_int_t xm_process_kill(lua_State* lua);
xu_int_t xm_process_close(lua_State* lua);

// the fwatcher functions
xu_int_t xm_fwatcher_open(lua_State* lua);
xu_int_t xm_fwatcher_add(lua_State* lua);
xu_int_t xm_fwatcher_remove(lua_State* lua);
xu_int_t xm_fwatcher_wait(lua_State* lua);
xu_int_t xm_fwatcher_close(lua_State* lua);

// the sandbox functions
xu_int_t xm_sandbox_interactive(lua_State* lua);

#ifdef XM_CONFIG_API_HAVE_READLINE
// the readline functions
xu_int_t xm_readline_readline(lua_State* lua);
xu_int_t xm_readline_history_list(lua_State* lua);
xu_int_t xm_readline_add_history(lua_State* lua);
xu_int_t xm_readline_clear_history(lua_State* lua);
#endif

// the semver functions
xu_int_t xm_semver_parse(lua_State* lua);
xu_int_t xm_semver_compare(lua_State* lua);
xu_int_t xm_semver_satisfies(lua_State* lua);
xu_int_t xm_semver_select(lua_State* lua);

// the libc functions
xu_int_t xm_libc_malloc(lua_State* lua);
xu_int_t xm_libc_free(lua_State* lua);
xu_int_t xm_libc_memcpy(lua_State* lua);
xu_int_t xm_libc_memmov(lua_State* lua);
xu_int_t xm_libc_memset(lua_State* lua);
xu_int_t xm_libc_strndup(lua_State* lua);
xu_int_t xm_libc_dataptr(lua_State* lua);
xu_int_t xm_libc_byteof(lua_State* lua);
xu_int_t xm_libc_setbyte(lua_State* lua);

// the tty functions
xu_int_t xm_tty_term_mode(lua_State* lua);

// the package functions
xu_int_t xm_package_loadxmi(lua_State* lua);

#ifdef XM_CONFIG_API_HAVE_CURSES
// register curses functions
xu_int_t xm_lua_curses_register(lua_State* lua, xu_char_t const* module);
#endif

// open cjson
xu_int_t luaopen_cjson(lua_State* l);

/* *******************************************************
 * globals
 */

// the os functions
static luaL_Reg const g_os_functions[] = {
    {"argv", xm_os_argv},         {"args", xm_os_args},         {"find", xm_os_find},
    {"link", xm_os_link},         {"isdir", xm_os_isdir},       {"rmdir", xm_os_rmdir},
    {"mkdir", xm_os_mkdir},       {"cpdir", xm_os_cpdir},       {"chdir", xm_os_chdir},
    {"mtime", xm_os_mtime},       {"sleep", xm_os_sleep},       {"mclock", xm_os_mclock},
    {"curdir", xm_os_curdir},     {"tmpdir", xm_os_tmpdir},     {"islink", xm_os_islink},
    {"isfile", xm_os_isfile},     {"touch", xm_os_touch},       {"rmfile", xm_os_rmfile},
    {"cpfile", xm_os_cpfile},     {"fscase", xm_os_fscase},     {"rename", xm_os_rename},
    {"exists", xm_os_exists},     {"setenv", xm_os_setenv},     {"getenv", xm_os_getenv},
    {"getenvs", xm_os_getenvs},   {"cpuinfo", xm_os_cpuinfo},   {"meminfo", xm_os_meminfo},
    {"readlink", xm_os_readlink}, {"emptydir", xm_os_emptydir}, {"strerror", xm_os_strerror},
    {"syserror", xm_os_syserror}, {"filesize", xm_os_filesize}, {"getwinsize", xm_os_getwinsize},
    {"getpid", xm_os_getpid},     {"signal", xm_os_signal},     {"uid", xm_os_uid},
    {"gid", xm_os_gid},           {"getown", xm_os_getown},     {xu_null, xu_null}};

// the io functions
static luaL_Reg const g_io_functions[] = {{"stdfile", xm_io_stdfile},
                                          {"file_open", xm_io_file_open},
                                          {"file_read", xm_io_file_read},
                                          {"file_readable", xm_io_file_readable},
                                          {"file_seek", xm_io_file_seek},
                                          {"file_size", xm_io_file_size},
                                          {"file_write", xm_io_file_write},
                                          {"file_flush", xm_io_file_flush},
                                          {"file_isatty", xm_io_file_isatty},
                                          {"file_close", xm_io_file_close},
                                          {"file_rawfd", xm_io_file_rawfd},
                                          {"filelock_open", xm_io_filelock_open},
                                          {"filelock_lock", xm_io_filelock_lock},
                                          {"filelock_trylock", xm_io_filelock_trylock},
                                          {"filelock_unlock", xm_io_filelock_unlock},
                                          {"filelock_close", xm_io_filelock_close},
                                          {"socket_open", xm_io_socket_open},
                                          {"socket_rawfd", xm_io_socket_rawfd},
                                          {"socket_peeraddr", xm_io_socket_peeraddr},
                                          {"socket_wait", xm_io_socket_wait},
                                          {"socket_bind", xm_io_socket_bind},
                                          {"socket_ctrl", xm_io_socket_ctrl},
                                          {"socket_listen", xm_io_socket_listen},
                                          {"socket_accept", xm_io_socket_accept},
                                          {"socket_connect", xm_io_socket_connect},
                                          {"socket_send", xm_io_socket_send},
                                          {"socket_sendto", xm_io_socket_sendto},
                                          {"socket_sendfile", xm_io_socket_sendfile},
                                          {"socket_recv", xm_io_socket_recv},
                                          {"socket_recvfrom", xm_io_socket_recvfrom},
                                          {"socket_kill", xm_io_socket_kill},
                                          {"socket_close", xm_io_socket_close},
                                          {"pipe_open", xm_io_pipe_open},
                                          {"pipe_openpair", xm_io_pipe_openpair},
                                          {"pipe_close", xm_io_pipe_close},
                                          {"pipe_read", xm_io_pipe_read},
                                          {"pipe_write", xm_io_pipe_write},
                                          {"pipe_wait", xm_io_pipe_wait},
                                          {"pipe_connect", xm_io_pipe_connect},
                                          {"poller_insert", xm_io_poller_insert},
                                          {"poller_modify", xm_io_poller_modify},
                                          {"poller_remove", xm_io_poller_remove},
                                          {"poller_spank", xm_io_poller_spank},
                                          {"poller_support", xm_io_poller_support},
                                          {"poller_wait", xm_io_poller_wait},
                                          {xu_null, xu_null}};

// the path functions
static luaL_Reg const g_path_functions[] = {{"relative", xm_path_relative},       {"absolute", xm_path_absolute},
                                            {"translate", xm_path_translate},     {"directory", xm_path_directory},
                                            {"is_absolute", xm_path_is_absolute}, {xu_null, xu_null}};

// the hash functions
static luaL_Reg const g_hash_functions[] = {{"uuid4", xm_hash_uuid4},
                                            {"sha", xm_hash_sha},
                                            {"md5", xm_hash_md5},
                                            {"xxhash", xm_hash_xxhash},
                                            {xu_null, xu_null}};

// the base64 functions
static luaL_Reg const g_base64_functions[] = {
    {"encode", xm_base64_encode}, {"decode", xm_base64_decode}, {xu_null, xu_null}};

// the lz4 functions
static luaL_Reg const g_lz4_functions[] = {{"compress", xm_lz4_compress},
                                           {"decompress", xm_lz4_decompress},
                                           {"block_compress", xm_lz4_block_compress},
                                           {"block_decompress", xm_lz4_block_decompress},
                                           {"compress_file", xm_lz4_compress_file},
                                           {"decompress_file", xm_lz4_decompress_file},
                                           {"compress_stream_open", xm_lz4_compress_stream_open},
                                           {"compress_stream_read", xm_lz4_compress_stream_read},
                                           {"compress_stream_write", xm_lz4_compress_stream_write},
                                           {"compress_stream_close", xm_lz4_compress_stream_close},
                                           {"decompress_stream_open", xm_lz4_decompress_stream_open},
                                           {"decompress_stream_read", xm_lz4_decompress_stream_read},
                                           {"decompress_stream_write", xm_lz4_decompress_stream_write},
                                           {"decompress_stream_close", xm_lz4_decompress_stream_close},
                                           {xu_null, xu_null}};

// the bloom filter functions
static luaL_Reg const g_bloom_filter_functions[] = {
    {"open", xm_bloom_filter_open}, {"close", xm_bloom_filter_close},       {"clear", xm_bloom_filter_clear},
    {"data", xm_bloom_filter_data}, {"size", xm_bloom_filter_size},         {"get", xm_bloom_filter_get},
    {"set", xm_bloom_filter_set},   {"data_set", xm_bloom_filter_data_set}, {xu_null, xu_null}};

// the string functions
static luaL_Reg const g_string_functions[] = {{"trim", xm_string_trim},
                                              {"split", xm_string_split},
                                              {"lastof", xm_string_lastof},
                                              {"convert", xm_string_convert},
                                              {"endswith", xm_string_endswith},
                                              {"startswith", xm_string_startswith},
                                              {xu_null, xu_null}};

// the process functions
static luaL_Reg const g_process_functions[] = {{"open", xm_process_open},   {"openv", xm_process_openv},
                                               {"wait", xm_process_wait},   {"kill", xm_process_kill},
                                               {"close", xm_process_close}, {xu_null, xu_null}};

// the fwatcher functions
static luaL_Reg const g_fwatcher_functions[] = {{"open", xm_fwatcher_open},     {"add", xm_fwatcher_add},
                                                {"remove", xm_fwatcher_remove}, {"wait", xm_fwatcher_wait},
                                                {"close", xm_fwatcher_close},   {xu_null, xu_null}};

// the sandbox functions
static luaL_Reg const g_sandbox_functions[] = {{"interactive", xm_sandbox_interactive}, {xu_null, xu_null}};

#ifdef XM_CONFIG_API_HAVE_READLINE
// the readline functions
static luaL_Reg const g_readline_functions[] = {{"readline", xm_readline_readline},
                                                {"history_list", xm_readline_history_list},
                                                {"add_history", xm_readline_add_history},
                                                {"clear_history", xm_readline_clear_history},
                                                {xu_null, xu_null}};
#endif

// the semver functions
static luaL_Reg const g_semver_functions[] = {{"parse", xm_semver_parse},
                                              {"compare", xm_semver_compare},
                                              {"satisfies", xm_semver_satisfies},
                                              {"select", xm_semver_select},
                                              {xu_null, xu_null}};

// the libc functions
static luaL_Reg const g_libc_functions[] = {{"malloc", xm_libc_malloc},   {"free", xm_libc_free},
                                            {"memcpy", xm_libc_memcpy},   {"memset", xm_libc_memset},
                                            {"memmov", xm_libc_memmov},   {"strndup", xm_libc_strndup},
                                            {"dataptr", xm_libc_dataptr}, {"byteof", xm_libc_byteof},
                                            {"setbyte", xm_libc_setbyte}, {xu_null, xu_null}};

// the tty functions
static luaL_Reg const g_tty_functions[] = {{"term_mode", xm_tty_term_mode}, {xu_null, xu_null}};

// the package functions
static luaL_Reg const g_package_functions[] = {{"loadxmi", xm_package_loadxmi}, {xu_null, xu_null}};

// the lua global instance for signal handler
static lua_State* g_lua = xu_null;

/* *******************************************************
 * private implementation
 */
static xu_bool_t xm_engine_save_arguments(xm_engine_t* engine, xu_int_t argc, xu_char_t** argv, xu_char_t** taskargv)
{
    // check
    xu_assert_and_check_return_val(engine && engine->lua && argc >= 1 && argv, xu_false);

    // put a new table into the stack
    lua_newtable(engine->lua);

    // patch the task arguments list
    if (taskargv)
    {
        xu_char_t** taskarg = taskargv;
        while (*taskarg)
        {
            lua_pushstring(engine->lua, *taskarg);
            lua_rawseti(engine->lua, -2, (int)lua_objlen(engine->lua, -2) + 1);
            taskarg++;
        }
    }

    // save all arguments to the new table
    xu_int_t i = 0;
    for (i = 1; i < argc; i++)
    {
        lua_pushstring(engine->lua, argv[i]);
        lua_rawseti(engine->lua, -2, (int)lua_objlen(engine->lua, -2) + 1);
    }

    // _ARGV = table_new
    lua_setglobal(engine->lua, "_ARGV");
    return xu_true;
}

static xu_size_t xm_engine_get_program_file(xm_engine_t* engine, xu_char_t* path, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(engine && path && maxn, xu_false);

    xu_bool_t ok = xu_false;
    do
    {
        // get it from the environment variable first
        if (xu_environment_first("XMAKE_PROGRAM_FILE", path, maxn) && tb_file_info(path, xu_null))
        {
            ok = xu_true;
            break;
        }

#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)
        /*
         * _NSGetExecutablePath() copies the path of the main executable into the buffer. The bufsize parameter
         * should initially be the size of the buffer.  The function returns 0 if the path was successfully copied,
         * and *bufsize is left unchanged. It returns -1 if the buffer is not large enough, and *bufsize is set
         * to the size required.
         *
         * Note that _NSGetExecutablePath will return "a path" to the executable not a "real path" to the executable.
         * That is the path may be a symbolic link and not the real file. With deep directories the total bufsize
         * needed could be more than MAXPATHLEN.
         */
        xu_uint32_t bufsize = (xu_uint32_t)maxn;
        if (!_NSGetExecutablePath(path, &bufsize)) ok = xu_true;
#elif defined(XM_PROC_SELF_FILE)
        // get the executable file path as program directory
        ssize_t size = readlink(XM_PROC_SELF_FILE, path, (size_t)maxn);
        if (size > 0 && size < maxn)
        {
            path[size] = '\0';
            ok         = xu_true;
        }
#elif defined(XU_CONFIG_OS_BSD) && defined(KERN_PROC_PATHNAME)
        // only for FreeBSD and OpenBSD, https://github.com/xmake-io/xmake/issues/2948
        xu_int_t mib[4];
        mib[0]      = CTL_KERN;
        mib[1]      = KERN_PROC;
        mib[2]      = KERN_PROC_PATHNAME;
        mib[3]      = -1;
        size_t size = maxn;
        if (sysctl(mib, 4, path, &size, xu_null, 0) == 0 && size < maxn)
        {
            path[size] = '\0';
            ok         = xu_true;
        }
#elif defined(XU_CONFIG_OS_HAIKU)
        int32      cookie = 0;
        image_info info;
        while (get_next_image_info(B_CURRENT_TEAM, &cookie, &info) == B_OK)
        {
            if (info.type == B_APP_IMAGE)
            {
                xu_strlcpy(path, info.name, maxn);
                ok = xu_true;
                break;
            }
        }
#else
        static xu_char_t const* s_paths[] = {"~/.local/bin/xmake", "/usr/local/bin/xmake", "/usr/bin/xmake"};
        for (xu_size_t i = 0; i < xu_arrayn(s_paths); i++)
        {
            xu_char_t const* p = s_paths[i];
            if (tb_file_info(p, xu_null))
            {
                xu_strlcpy(path, p, maxn);
                ok = xu_true;
                break;
            }
        }
#endif

    } while (0);

    // ok?
    if (ok)
    {
        // trace
        tb_trace_d("programfile: %s", path);

        // save the directory to the global variable: _PROGRAM_FILE
        lua_pushstring(engine->lua, path);
        lua_setglobal(engine->lua, "_PROGRAM_FILE");
    }
    return ok;
}

static xu_bool_t xm_engine_get_program_directory(xm_engine_t* engine, xu_char_t* path, xu_size_t maxn,
                                                 xu_char_t const* programfile)
{
    // check
    xu_assert_and_check_return_val(engine && path && maxn, xu_false);

    xu_bool_t ok = xu_false;
    do
    {
        // get it from the environment variable first
        xu_char_t data[TB_PATH_MAXN] = {0};
        if (xu_environment_first("XMAKE_PROGRAM_DIR", data, sizeof(data)) && tb_path_absolute(data, path, maxn))
        {
            ok = xu_true;
            break;
        }

        // get it from program file path
        if (programfile)
        {
            // get real program file path from the symbol link
#if !defined(XU_CONFIG_OS_IOS)
            xu_char_t programpath[TB_PATH_MAXN];
            tb_long_t size = readlink(programfile, programpath, sizeof(programpath));
            if (size >= 0 && size < sizeof(programpath))
            {
                programpath[size] = '\0';

                // soft link to relative path? fix it
                if (!tb_path_is_absolute(programpath))
                {
                    xu_char_t        buff[TB_PATH_MAXN];
                    xu_char_t const* rootdir = tb_path_directory(programfile, buff, sizeof(buff));
                    if (rootdir && tb_path_absolute_to(rootdir, programpath, path,
                                                       maxn)) // @note path and programfile are same buffer
                        xu_strlcpy(programpath, path, maxn);
                }
            }
            else
                xu_strlcpy(programpath, programfile, sizeof(programpath));
#else
            xu_char_t const* programpath = programfile;
#endif

            // get the root directory
            xu_char_t        data[TB_PATH_MAXN];
            xu_char_t const* rootdir = tb_path_directory(programpath, data, sizeof(data));
            tb_assert_and_check_break(rootdir);

            // init share/name sub-directory
            xu_char_t sharedir[128];
            tb_snprintf(sharedir, sizeof(sharedir), "../share/%s", engine->name);

            // find the program (lua) directory
            xu_size_t        i;
            tb_file_info_t   info;
            xu_char_t        scriptpath[TB_PATH_MAXN];
            xu_char_t const* subdirs[] = {".", sharedir};
            for (i = 0; i < xu_arrayn(subdirs); i++)
            {
                // get program directory
                if (tb_path_absolute_to(rootdir, subdirs[i], path, maxn) &&
                    tb_path_absolute_to(path, "core/_xmake_main.lua", scriptpath, sizeof(scriptpath)) &&
                    tb_file_info(scriptpath, &info) && info.type == TB_FILE_TYPE_FILE)
                {
                    ok = xu_true;
                    break;
                }
            }
        }

    } while (0);

    // ok?
    if (ok)
    {
        // trace
        tb_trace_d("programdir: %s", path);

        // save the directory to the global variable: _PROGRAM_DIR
        lua_pushstring(engine->lua, path);
        lua_setglobal(engine->lua, "_PROGRAM_DIR");
    }

    // ok?
    return ok;
}

static xu_bool_t xm_engine_get_project_directory(xm_engine_t* engine, xu_char_t* path, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(engine && path && maxn, xu_false);

    xu_bool_t ok = xu_false;
    do
    {
        // attempt to get it from the environment variable first
        xu_char_t data[TB_PATH_MAXN] = {0};
        if (!xu_environment_first("XMAKE_PROJECT_DIR", data, sizeof(data)) || !tb_path_absolute(data, path, maxn))
        {
            // get it from the current directory
            if (!tb_directory_current(path, maxn)) break;
        }

        // trace
        tb_trace_d("project: %s", path);

        // save the directory to the global variable: _PROJECT_DIR
        lua_pushstring(engine->lua, path);
        lua_setglobal(engine->lua, "_PROJECT_DIR");

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok) tb_printf("error: not found the project directory!\n");

    // ok?
    return ok;
}

#if defined(SIGINT)
static xu_void_t xm_engine_dump_traceback(lua_State* lua)
{
    // @note it's not safe, but it doesn't matter, we're just trying to get the stack backtrace for debugging
    lua_getglobal(lua, "debug");
    lua_getfield(lua, -1, "traceback");
    lua_replace(lua, -2);
    lua_pushvalue(lua, 1);
    lua_call(lua, 1, 1);
    tb_trace_i("%s", lua_tostring(lua, -1));
}
#endif

#if defined(SIGINT)
static xu_void_t xm_engine_signal_handler(xu_int_t signo)
{
    if (signo == SIGINT && g_lua)
    {
        xm_engine_dump_traceback(g_lua);
        tb_abort();
    }
}
#endif

static xu_void_t xm_engine_init_host(xm_engine_t* engine)
{
    // check
    tb_assert_and_check_return(engine && engine->lua);

    // init system host
    xu_char_t const* syshost = xu_null;
#if defined(XU_CONFIG_OS_MACOSX)
    syshost = "macosx";
#elif defined(XU_CONFIG_OS_LINUX)
    syshost = "linux";
#elif defined(XU_CONFIG_OS_BSD)
    syshost = "bsd";
#elif defined(XU_CONFIG_OS_IOS)
    syshost = "ios";
#elif defined(XU_CONFIG_OS_ANDROID)
    syshost = "android";
#elif defined(XU_CONFIG_OS_HAIKU)
    syshost = "haiku";
#endif
    lua_pushstring(engine->lua, syshost ? syshost : "unknown");
    lua_setglobal(engine->lua, "_HOST");

    // init subsystem host
    xu_char_t const* subhost = syshost;
    lua_pushstring(engine->lua, subhost ? subhost : "unknown");
    lua_setglobal(engine->lua, "_SUBHOST");
}

static __xu_inline__ xu_char_t const* xm_engine_xmake_arch()
{
    xu_char_t const* arch = xu_null;
#if defined(TB_ARCH_x64)
    arch = "x86_64";
#elif defined(TB_ARCH_x86)
    arch = "i386";
#elif defined(TB_ARCH_ARM64)
    arch = "arm64";
#else
    arch = TB_ARCH_STRING;
#endif
    return arch;
}

static xu_void_t xm_engine_init_arch(xm_engine_t* engine)
{
    // check
    tb_assert_and_check_return(engine && engine->lua);

    // init xmake arch
    xu_char_t const* xmakearch = xm_engine_xmake_arch();
    lua_pushstring(engine->lua, xmakearch);
    lua_setglobal(engine->lua, "_XMAKE_ARCH");

    // init system architecture
    xu_char_t const* sysarch = xu_null;
    if (!sysarch) sysarch = xmakearch;
    lua_pushstring(engine->lua, sysarch);
    lua_setglobal(engine->lua, "_ARCH");

    // init subsystem architecture
    xu_char_t const* subarch = sysarch;
    lua_pushstring(engine->lua, subarch);
    lua_setglobal(engine->lua, "_SUBARCH");
}

static xu_void_t xm_engine_init_features(xm_engine_t* engine)
{
    // check
    tb_assert_and_check_return(engine && engine->lua);

    // init features
    lua_newtable(engine->lua);

    // get path seperator
    lua_pushstring(engine->lua, "path_sep");
    lua_pushstring(engine->lua, "/");
    lua_settable(engine->lua, -3);

    // get environment path seperator
    lua_pushstring(engine->lua, "path_envsep");
    lua_pushstring(engine->lua, ":");
    lua_settable(engine->lua, -3);

    lua_setglobal(engine->lua, "_FEATURES");
}

static xu_void_t xm_engine_init_signal(xm_engine_t* engine)
{
    // we enable it to catch the current lua stack in ctrl-c signal handler if XMAKE_PROFILE=stuck
    xu_char_t data[64] = {0};
    if (!xu_environment_first("XMAKE_PROFILE", data, sizeof(data)) || tb_strcmp(data, "stuck")) return;

    g_lua = engine->lua;
#if defined(SIGINT)
    signal(SIGINT, xm_engine_signal_handler);
#endif
}

#if XM_HOOK_LUA_MEMALLOC
static tb_pointer_t xm_engine_lua_realloc(tb_pointer_t udata, tb_pointer_t data, size_t osize, size_t nsize)
{
    tb_pointer_t ptr = xu_null;
    if (nsize == 0 && data)
        tb_free(data);
    else if (!data)
        ptr = tb_malloc((xu_size_t)nsize);
    else if (nsize != osize)
        ptr = tb_ralloc(data, (xu_size_t)nsize);
    else
        ptr = data;
    return ptr;
}
#endif

/* *******************************************************
 * implementation
 *
 */
xm_engine_ref_t xm_engine_init(xu_char_t const* name, xm_engine_lni_initializer_cb_t lni_initializer)
{
    xu_bool_t    ok     = xu_false;
    xm_engine_t* engine = xu_null;
    do
    {
        // init self
        engine = tb_malloc0_type(xm_engine_t);
        tb_assert_and_check_break(engine);

        // init name
        xu_strlcpy(engine->name, name, sizeof(engine->name));

        // init lua
        engine->lua = luaL_newstate();
        tb_assert_and_check_break(engine->lua);

#if XM_HOOK_LUA_MEMALLOC
        // hook lua memory
        lua_setallocf(engine->lua, xm_engine_lua_realloc, engine->lua);
#endif

        // open lua libraries
        luaL_openlibs(engine->lua);

        // bind os functions
        xm_lua_register(engine->lua, "os", g_os_functions);

        // bind io functions
        xm_lua_register(engine->lua, "io", g_io_functions);

        // bind path functions
        xm_lua_register(engine->lua, "path", g_path_functions);

        // bind hash functions
        xm_lua_register(engine->lua, "hash", g_hash_functions);

        // bind lz4 functions
        xm_lua_register(engine->lua, "lz4", g_lz4_functions);

        // bind bloom filter functions
        xm_lua_register(engine->lua, "bloom_filter", g_bloom_filter_functions);

        // bind base64 functions
        xm_lua_register(engine->lua, "base64", g_base64_functions);

        // bind string functions
        xm_lua_register(engine->lua, "string", g_string_functions);

        // bind process functions
        xm_lua_register(engine->lua, "process", g_process_functions);

        // bind fwatcher functions
        xm_lua_register(engine->lua, "fwatcher", g_fwatcher_functions);

        // bind sandbox functions
        xm_lua_register(engine->lua, "sandbox", g_sandbox_functions);

#ifdef XM_CONFIG_API_HAVE_READLINE
        // bind readline functions
        xm_lua_register(engine->lua, "readline", g_readline_functions);
#endif

        // bind semver functions
        xm_lua_register(engine->lua, "semver", g_semver_functions);

        // bind libc functions
        xm_lua_register(engine->lua, "libc", g_libc_functions);

        // bind tty functions
        xm_lua_register(engine->lua, "tty", g_tty_functions);

        // bind package functions
        xm_lua_register(engine->lua, "package", g_package_functions);

#ifdef XM_CONFIG_API_HAVE_CURSES
        // bind curses
        xm_lua_curses_register(engine->lua, "curses");
#endif

#ifdef XM_CONFIG_API_HAVE_LUA_CJSON
        // bind cjson
        luaopen_cjson(engine->lua);
        lua_setglobal(engine->lua, "cjson");
#endif

        // init host
        xm_engine_init_host(engine);

        // init architecture
        xm_engine_init_arch(engine);

        // init features
        xm_engine_init_features(engine);

        // init signal
        xm_engine_init_signal(engine);

        // get version
        xu_version_t const* version = xm_version();
        tb_assert_and_check_break(version);

        // init version string
        xu_char_t version_cstr[256] = {0};
        if (tb_strcmp(XM_CONFIG_VERSION_BRANCH, "") && tb_strcmp(XM_CONFIG_VERSION_COMMIT, ""))
            tb_snprintf(version_cstr, sizeof(version_cstr), "%u.%u.%u+%s.%s", version->major, version->minor,
                        version->alter, XM_CONFIG_VERSION_BRANCH, XM_CONFIG_VERSION_COMMIT);
        else
            tb_snprintf(version_cstr, sizeof(version_cstr), "%u.%u.%u+%llu", version->major, version->minor,
                        version->alter, version->build);
        lua_pushstring(engine->lua, version_cstr);
        lua_setglobal(engine->lua, "_VERSION");

        // init short version string
        tb_snprintf(version_cstr, sizeof(version_cstr), "%u.%u.%u", version->major, version->minor, version->alter);
        lua_pushstring(engine->lua, version_cstr);
        lua_setglobal(engine->lua, "_VERSION_SHORT");

        // init engine name
        lua_pushstring(engine->lua, name ? name : "xmake");
        lua_setglobal(engine->lua, "_NAME");

        // init git source
        lua_pushstring(engine->lua,
                       tb_strcmp(XM_CONFIG_VERSION_GIT_SOURCE, "") ? XM_CONFIG_VERSION_GIT_SOURCE : "gitee.com");
        lua_setglobal(engine->lua, "_GIT_SOURCE");

        // use luajit as runtime?
#ifdef USE_LUAJIT
        lua_pushboolean(engine->lua, xu_true);
#else
        lua_pushboolean(engine->lua, xu_false);
#endif
        lua_setglobal(engine->lua, "_LUAJIT");

        // init namespace: xmake
        lua_newtable(engine->lua);
        lua_setglobal(engine->lua, "xmake");

        /* do lua initializer and init namespace: _lni
         *
         * we can get the lni modules for _lni or `import("lib.lni.xxx")` in sandbox
         */
        lua_newtable(engine->lua);
        if (lni_initializer) lni_initializer((xm_engine_ref_t)engine, engine->lua);
        lua_setglobal(engine->lua, "_lni");
        ok = xu_true;

    } while (0);

    if (!ok)
    {
        if (engine) xm_engine_exit((xm_engine_ref_t)engine);
        engine = xu_null;
    }
    return (xm_engine_ref_t)engine;
}
xu_void_t xm_engine_exit(xm_engine_ref_t self)
{
    // check
    xm_engine_t* engine = (xm_engine_t*)self;
    tb_assert_and_check_return(engine);

    // exit lua
    if (engine->lua) lua_close(engine->lua);
    engine->lua = xu_null;

    // exit it
    tb_free(engine);
}
xu_int_t xm_engine_main(xm_engine_ref_t self, xu_int_t argc, xu_char_t** argv, xu_char_t** taskargv)
{
    // check
    xm_engine_t* engine = (xm_engine_t*)self;
    xu_assert_and_check_return_val(engine && engine->lua, -1);

    // save main arguments to the global variable: _ARGV
    if (!xm_engine_save_arguments(engine, argc, argv, taskargv)) return -1;

    // get the project directory
    xu_char_t path[TB_PATH_MAXN] = {0};
    if (!xm_engine_get_project_directory(engine, path, sizeof(path))) return -1;

    // get the program file
    if (!xm_engine_get_program_file(engine, path, sizeof(path))) return -1;

    // get the program directory
    if (!xm_engine_get_program_directory(engine, path, sizeof(path), path)) return -1;

    // append the main script path
    tb_strcat(path, "/core/_xmake_main.lua");

    // exists this script?
    if (!tb_file_info(path, xu_null))
    {
        tb_printf("not found main script: %s\n", path);
        return -1;
    }

    // trace
    tb_trace_d("main: %s", path);

    // load and execute the main script
    if (luaL_dofile(engine->lua, path))
    {
        tb_printf("error: %s\n", lua_tostring(engine->lua, -1));
        return -1;
    }

    // set the error function
    lua_getglobal(engine->lua, "debug");
    lua_getfield(engine->lua, -1, "traceback");

    // call the main function
    lua_getglobal(engine->lua, "_xmake_main");
    if (lua_pcall(engine->lua, 0, 1, -2))
    {
        tb_printf("error: %s\n", lua_tostring(engine->lua, -1));
        return -1;
    }

    // get the error code
    return (xu_int_t)lua_tonumber(engine->lua, -1);
}
xu_void_t xm_engine_register(xm_engine_ref_t self, xu_char_t const* module, luaL_Reg const funcs[])
{
    // check
    xm_engine_t* engine = (xm_engine_t*)self;
    tb_assert_and_check_return(engine && engine->lua && module && funcs);

    // do register
    lua_pushstring(engine->lua, module);
    lua_newtable(engine->lua);
    xm_lua_register(engine->lua, xu_null, funcs);
    lua_rawset(engine->lua, -3);
}
xu_int_t xm_engine_run(xu_char_t const* name, xu_int_t argc, xu_char_t** argv, xu_char_t** taskargv,
                       xm_engine_lni_initializer_cb_t lni_initializer)
{
    xu_int_t ok = -1;
    if (xm_init())
    {
        xm_engine_ref_t engine = xm_engine_init(name, lni_initializer);
        if (engine)
        {
            ok = xm_engine_main(engine, argc, argv, taskargv);
            xm_engine_exit(engine);
        }
        xm_exit();
    }
    return ok;
}
