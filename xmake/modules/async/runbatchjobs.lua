import("core.base.scheduler")

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
                local co = scheduler.co_start_withopt({name = group_name .. '/' .. tostring(i)}, function(i)
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
                                    for _, obj in waitobjs:keys() do
                                        -- TODO: kill pipe is not supported now
                                        if obj.kill then
                                            obj:kill()
                                        end
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
