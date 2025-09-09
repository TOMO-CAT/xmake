import("core.base.scheduler")
import("core.base.option")

function main(name, jobs, opt)
    local total = opt.total or (type(jobs) == "table" and jobs:size()) or 1
    local comax = tonumber(opt.comax) or math.min(total, 4)
    local group_name = name
    assert(jobs, "runbatchjobs: no jobs!")

    -- run batchjobs
    local index = 0
    local count = 0
    local stop = false
    local abort = false
    local abort_errors
    local progress_wrapper = {}
    progress_wrapper.current = function() return count end
    progress_wrapper.total = function() return total end
    progress_wrapper.percent = function()
        if total and total > 0 then
            return math.floor((count * 100) / total)
        else
            return 0
        end
    end
    debug.setmetatable(progress_wrapper, {
        __tostring = function()
            return string.format("%d%%", progress_wrapper.percent())
        end
    })

    scheduler.enter_co_group(group_name)
    local co_group = scheduler.co_group(group_name)

    while index < total do
        while #co_group < comax do
            -- get a free job from the jobpool
            local job = jobs:getfree()
            if job and job.run then
                local jobfunc = job.run
                local jobname = job.name

                -- start this job
                index = index + 1
                local co = scheduler.co_start_withopt({name = group_name .. '/' .. jobname}, function(i)
                    try {
                        function()
                            if stop then
                                return
                            end
                            if opt.curdir then
                                os.cd(opt.curdir)
                            end
                            count = count + 1
                            jobfunc(i, total, {progress = progress_wrapper})
                        end, catch {
                            function(errors)
                                stop = true

                                -- we need re-throw this errors outside scheduler
                                abort = true
                                if abort_errors == nil then
                                    abort_errors = errors
                                end

                                -- kill all waited objects in this group
                                local waitobjs =
                                    scheduler.co_group_waitobjs(group_name)
                                if waitobjs:size() > 0 then
                                    -- 目前无法 kill 掉子进程的子进程, 所以我们选择和 cmake 一样 wait 所有并发任务结束, 如果用户 ctrl +c 也可能退干净
                                    -- 主要是 gcc 进程会创建子进程, 如果 xmake 编译出错这里 kill 只能杀掉 gcc 进程, 无法杀掉 gcc 进程创建的子进程
                                    -- @see https://github.com/xmake-io/xmake/issues/719
                                    utils.cprint("${bright yellow}Encountered some errors, waiting for [%d] unfinished jobs (press Ctrl+C to abort)${clear}", waitobjs:size())

                                    for _, obj in waitobjs:keys() do
                                        -- TODO: kill pipe is not supported now
                                        -- if obj.kill then
                                        --      obj:kill()
                                        -- end

                                        -- 线上机器编译参数比较复杂, 核数比较多, 打印的日志太多了暂时关闭
                                        -- if option.get("verbose") or option.get("diagnosis") then
                                        --     utils.cprint("${bright yellow}Unfinished jobs${clear} [%s]", obj:desc())
                                        -- end
                                    end
                                end
                            end
                        }, finally {
                            function()
                                if job then jobs:remove(job) end
                            end
                        }
                    }
                end, index)

                -- add this coroutine to co_group
                scheduler.add_to_co_group(group_name, co)
            else
                -- if not job then
                --     os.sleep(20)
                -- end
                break
            end
        end

        -- wait for free jobs
        scheduler.co_group_wait(group_name, {limit = 1})

    end

    -- wait all jobs exited
    scheduler.co_group_wait(group_name)
    scheduler.leave_co_group(group_name)

    -- do exit callback
    if opt.on_exit then opt.on_exit(abort_errors) end

    -- re-throw abort errors
    --
    -- @note we cannot throw it in coroutine,
    -- because his causes a direct exit from the entire runloop and
    -- a quick escape from nested try-catch blocks and coroutines groups.
    -- so we can not catch runjobs errors, e.g. build fails
    if abort then raise(abort_errors) end
end
