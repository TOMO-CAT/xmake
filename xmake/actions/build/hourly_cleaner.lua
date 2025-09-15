import("core.base.option")
import("core.base.process")
import("core.base.global")
import("core.base.colors")
import("core.base.json")
import("core.package.package")

-- 仿造 utils.cprint(format, ...)
function _log_info(format, ...)
    assert(format)
    local message = string.tryformat(format, ...)
    print("[%s][HOURLY-CLEANER][INFO] %s", os.date("%Y-%m-%d_%H:%M:%S", os.time()), message)
end

function _log_warn(format, ...)
    assert(format)
    local message = string.tryformat(format, ...)
    print("[%s][HOURLY-CLEANER][WARN] %s", os.date("%Y-%m-%d_%H:%M:%S", os.time()), message)
end

function _cleanup_file(filepath)
    print("cleanup [%s] ...", filepath)
    os.tryrm(filepath)
end

-- 检查 filelock 是否过期, 这意味着我们可以删除对应的 package 和 cache 目录
function _filelock_expired(filelock_path, pkg_retain_hours)
    assert(filelock_path)
    assert(pkg_retain_hours)

    local content = io.load(filelock_path)
    if content then
        if not content.time then
            _log_warn("missed `time` field in filelock file [%s]:", filelock_path)
            print(io.readfile(filelock_path))
            return false
        else
            local passed_hours = ((os.time() - content.time) / 3600)
            if passed_hours >= pkg_retain_hours then
                _log_info("filelock [%s] have not been accessed for [%.5f] hours and is considered expired.", filelock_path, passed_hours)
                return true
            else
                _log_info("filelock [%s] is still in use before [%.5f] hours", filelock_path, passed_hours)
            end
        end
    end
    return false
end

function _cleanup_impl()
    -- 没设置的话默认清理 30 天未用到的 package / cache
    local pkg_retain_hours = os.getenv("XMAKE_PKG_RETAIN_HOURS") or (30 * 24)
    _log_info("start hourly cleaner with XMAKE_PKG_RETAIN_HOURS [%d] ...", pkg_retain_hours)

    -- 遍历所有的 package filelock, 会自动删除超过 pkg_retain_hours 的目录
    local pkg_filelock_dir = package.filelockdir()
    for _, filelock_path in ipairs(os.files(path.join(pkg_filelock_dir, "*.lock"))) do
        if not os.isfile(filelock_path) then
            -- 清理掉异常的 filelock 文件
            _log_warn("invalid filelock file [%s]", filelock_path)
            _cleanup_file(filelock_path)
        else
            local filelock_name = path.basename(filelock_path)
            local splitinfo = filelock_name:split("__", {plain = true, strict = true})
            if #splitinfo ~= 3 then
                _log_warn("invalid filelock pattern [%s]", filelock_name)
                _cleanup_file(filelock_path)
            else
                _log_info("process filelock [%s]", filelock_path)
                if _filelock_expired(filelock_path, pkg_retain_hours) then
                    local filelock = io.openlock(filelock_path)
                    if not filelock then
                        _log_warn("cannot create filelock for [%s]", filelock_path)
                        _cleanup_file(filelock_path)
                    else
                        -- 可以通过 lsof 查看谁在占用 filelock
                        -- 尝试加锁准备删除过期 package
                        if not filelock:trylock({dump_timestamp = false}) then
                            -- 加锁失败说明有人在用, 不应该删除该文件锁
                            _log_info("lock file [%s] is still in use", filelock_path)
                        else
                            _log_info("try lock file [%s] success", filelock_path)
                            -- 加锁后 double check 文件锁是否过期, 避免出现临界区访问
                            if not _filelock_expired(filelock_path, pkg_retain_hours) then
                                _log_warn("double check filelock [%s], this filelock still in use", filelock_path)
                            else
                                _log_info("filelock [%s] expired, need to do some cleanup jobs", filelock_path)
                                -- 先删除 installdir
                                local installdir = path.join(package.installdir(), splitinfo[1], splitinfo[2], splitinfo[3])
                                _cleanup_file(installdir)
                                -- 再删除 cacahedir
                                local cachedir = path.join(package.cachedir(), splitinfo[1], splitinfo[2], splitinfo[3])
                                _cleanup_file(cachedir)
                                -- 最后删除自身文件锁 (删除后再解锁是可以的, 文件锁是绑定到 inode 的)
                                _cleanup_file(filelock_path)
                            end
                            -- 释放锁
                            filelock:unlock()
                            _log_info("release filelock [%s]", filelock_path)
                        end
                    end
                end
            end
        end
    end
end

function cleanup()
    local this_hour = os.date("%y%m%d%H")

    -- 每个小时执行一次, 判断是否执行过了
    local markfile = path.join(os.tmpdir(), "cleanup",  this_hour .. ".mark")
    if os.isfile(markfile) then
        return
    end

    -- 标记当前小时已经执行过了, 避免重复执行
    -- 并发场景仍然有可能进来, 但是后续的清理会上锁所以问题不大
    io.writefile(markfile, "ok")

    -- init argument list
    local argv = {"lua", "--diagnosis", path.join(os.scriptdir(), "hourly_cleaner.lua")}
    for _, name in ipairs({"root", "file", "project", "verbose", "diagnosis", "quiet", "yes", "confirm"}) do
        local value = option.get(name)
        if type(value) == "string" then
            table.insert(argv, "--" .. name .. "=" .. value)
        elseif value then
            table.insert(argv, "--" .. name)
        end
    end

    -- 尝试后台运行, 不要干扰主线程, 而且日志会写到 xmake tmp 目录的 hourly-cleaner.log 里
    try
    {
        function ()
            print(argv)
            process.openv(os.programfile(), argv, {stdout = path.join(os.tmpdir(), this_hour .. "-hourly-cleaner.log"), detach = true}):close()
        end
    }
end

function main()
    _cleanup_impl()
end
