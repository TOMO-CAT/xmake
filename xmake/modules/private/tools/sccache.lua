-- imports
import("core.project.config")
import("lib.detect.find_tool")
import("core.cache.memcache")
import("core.project.project")
import("core.base.option")

-- get sccache tool
function _sccache()
    local sccache = _g.sccache
    if sccache == nil and config.get("sccache") then
        sccache = find_tool("sccache")
        _g.sccache = sccache or false
    end
    return sccache or nil
end

-- get memcache
function _memcache()
    local cache = _g.memcache
    if not cache then
        cache = memcache.cache("sccache")
        _g.memcache = cache
    end
    return cache
end

-- is enabled?
function is_enabled(target)
    local key = tostring(target or "all")
    local result = _memcache():get2("enabled", key)
    if result == nil then
        -- check if sccache exists?
        if result == nil and not exists() then
            result = false
        end
        -- target may be option instance
        if result == nil and target and target.policy then
            result = target:policy("build.sccache")
        end
        if result == nil and os.isfile(os.projectfile()) then
            local policy = project.policy("build.sccache")
            if policy ~= nil then
                result = policy
            end
        end
        if result == nil then
            result = config.get("sccache")
        end
        result = result or false
        _memcache():set2("enabled", key)
    end
    return result
end

-- get approximate hit rate
function hitrate()
    local cache_hit_count = (_g.cache_hit_count or 0)
    local total_count = (_g.total_count or 0)
    if total_count > 0 then
        return math.floor(cache_hit_count * 100 / total_count)
    end
    return 0
end

-- report compile metrics
function report_metrics(sourcefile, compile_time_ms)
    _g.total_count = (_g.total_count or 0) + 1
    -- because we cannot get the hit rate for the current build from sccache,
    -- we assume that a cache hit will occur as long as the compilation time
    -- is less than 1 second.
    if compile_time_ms < 1000 then
        _g.cache_hit_count = (_g.cache_hit_count or 0) + 1
    end
    local file2compile_time = _g.file2compile_time or {}
    _g.file2compile_time = file2compile_time
    _g.file2compile_time[sourcefile] = compile_time_ms
    _g.compile_total_time = (_g.compile_total_time or 0) + compile_time_ms
end

-- dump sccache statistics
function dump_stats()
    local total_count = (_g.total_count or 0)
    local cache_hit_count = (_g.cache_hit_count or 0)
    local cache_miss_count = total_count - cache_hit_count
    local compile_total_time = (_g.compile_total_time or 0)

    -- build sccache stats
    if option.get("verbose") or option.get("diagnosis") then
        print("")
        cprint("${color.success}build sccache stats:")
        print("cache approximate hit rate: %d%%", hitrate())
        print("cache hit: %d", cache_hit_count)
        print("cache miss: %d", cache_miss_count)
        print("compile total time: %0.3fs", compile_total_time / 1000.0)
        print("")
    end

    -- file compile time stats
    if option.get("verbose") or option.get("diagnosis") then
        if _g.file2compile_time then
            cprint("${color.success}file compile time (top3) stats:")
            local sorted_file2compile_time = {}
            for key, value in pairs(_g.file2compile_time) do
                table.insert(sorted_file2compile_time, {key = key, value = value})
            end
            table.sort(sorted_file2compile_time, function(a, b) return a.value > b.value end)
            io.save(path.join(config.debugdir(), "file-compile-time.txt"), sorted_file2compile_time)
            -- print top 3 files with longest compile time
            for i = 1, math.min(3, #sorted_file2compile_time) do
                print("%s: %0.3fs", sorted_file2compile_time[i].key, sorted_file2compile_time[i].value / 1000.0)
            end
            print("")
        end
    end
end

-- exists sccache?
function exists()
    return _sccache() ~= nil
end

-- uses sccache to wrap the program and arguments
--
-- e.g. sccache program argv
--
function cmdargv(program, argv)

    -- uses sccache?
    local sccache = _sccache()
    if sccache then

        -- parse the filename and arguments, e.g. "xcrun -sdk macosx clang"
        if not os.isexec(program) then
            argv = table.join(program:split("%s"), argv)
        else
            table.insert(argv, 1, program)
        end
        return sccache.program, argv
    end
    return program, argv
end
