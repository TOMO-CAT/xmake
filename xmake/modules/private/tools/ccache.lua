--!A cross-platform build utility based on Lua
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--
-- Copyright (C) 2015-present, TBOOX Open Source Group.
--
-- @author      ruki
-- @file        ccache.lua
--

-- imports
import("core.project.config")
import("lib.detect.find_tool")
import("core.cache.memcache")
import("core.project.project")
import("core.base.option")

-- get ccache tool
function _ccache()
    local ccache = _g.ccache
    if ccache == nil and config.get("ccache") then
        ccache = find_tool("ccache")
        _g.ccache = ccache or false
    end
    return ccache or nil
end

-- get memcache
function _memcache()
    local cache = _g.memcache
    if not cache then
        cache = memcache.cache("ccache")
        _g.memcache = cache
    end
    return cache
end

-- is enabled?
function is_enabled(target)
    local key = tostring(target or "all")
    local result = _memcache():get2("enabled", key)
    if result == nil then
        -- check if ccache exist?
        if result == nil and not exists() then
            result = false
        end
        -- target may be option instance
        if result == nil and target and target.policy then
            result = target:policy("build.ccache")
        end
        if result == nil and os.isfile(os.projectfile()) then
            local policy = project.policy("build.ccache")
            if policy ~= nil then
                result = policy
            end
        end
        if result == nil then
            result = config.get("ccache")
        end
        result = result or false
        _memcache():set2("enabled", key)
    end
    return result
end

-- get hit rate
function hitrate()
    local cache_hit_count = (_g.cache_hit_count or 0)
    local total_count = (_g.total_count or 0)
    if total_count > 0 then
        return math.floor(cache_hit_count * 100 / total_count)
    end
    return 0
end

function report_metrics(sourcefile, compile_time_ms)
    _g.total_count = (_g.total_count or 0) + 1
    -- because we cannot get ccahce hit-rate, so we assume that a cache hit will occur
    -- as long as the compilation time is less than 1 second
    if compile_time_ms < 1000 then
        _g.cache_hit_count = (_g.cache_hit_count or 0) + 1
    end
    local file2compile_time = _g.file2compile_time or {}
    _g.file2compile_time = file2compile_time
    _g.file2compile_time[sourcefile] = compile_time_ms
    _g.compile_total_time = (_g.compile_total_time or 0) + compile_time_ms
end

function dump_stats()
    local total_count = (_g.total_count or 0)
    local cache_hit_count = (_g.cache_hit_count or 0)
    local cache_miss_count = total_count - cache_hit_count
    local compile_total_time = (_g.compile_total_time or 0)

    -- build ccache stats
    print("")
    cprint("${color.success}build ccache stats:")
    print("cache approximate hit rate: %d%%", hitrate())
    print("cache hit: %d", cache_hit_count)
    print("cache miss: %d", cache_miss_count)
    print("compile total time: %0.3fs", compile_total_time / 1000.0)
    print("")

    -- file compile time stats
    if _g.file2compile_time then
        cprint("${color.success}file compile time (top3) stats:")
        local sorted_file2compile_time = {}
        for key, value in pairs(_g.file2compile_time) do
            table.insert(sorted_file2compile_time, {key = key, value = value})
        end
        table.sort(sorted_file2compile_time, function(a, b) return a.value > b.value end)
        if option.get("verbose") or option.get("diagnosis") then
            io.save(path.join(config.debugdir(), "file-compile-time.txt"), sorted_file2compile_time)
        end
        -- print top 3 files with longest compile time
        for i = 1, math.min(3, #sorted_file2compile_time) do
            print("%s: %0.3fs", sorted_file2compile_time[i].key, sorted_file2compile_time[i].value / 1000.0)
        end
        print("")
    end
end

-- exists ccache?
function exists()
    return _ccache() ~= nil
end

-- uses ccache to wrap the program and arguments
--
-- e.g. ccache program argv
--
function cmdargv(program, argv)

    -- uses ccache?
    local ccache = _ccache()
    if ccache then

        -- parse the filename and arguments, e.g. "xcrun -sdk macosx clang"
        if not os.isexec(program) then
            argv = table.join(program:split("%s"), argv)
        else
            table.insert(argv, 1, program)
        end
        return ccache.program, argv
    end
    return program, argv
end
