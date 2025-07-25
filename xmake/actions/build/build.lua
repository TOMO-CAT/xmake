-- !A cross-platform build utility based on Lua
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
-- @file        build.lua
--
-- imports
import("core.base.option")
import("core.project.config")
import("core.project.project")
import("private.async.jobpool")
import("async.runjobs")
import("async.runbatchjobs")
import("private.utils.batchcmds")
import("core.base.hashset")
import("private.service.remote_cache.client", {alias = "remote_cache_client"})
import("private.service.distcc_build.client", {alias = "distcc_build_client"})

-- clean target for rebuilding
function _clean_target(target)
    if target:targetfile() then
        os.tryrm(target:symbolfile())
        os.tryrm(target:targetfile())
    end
end

-- add builtin batch jobs
function _add_batchjobs_builtin(batchjobs, rootjob, target)

    -- uses the rules script?
    local job, job_leaf
    for _, r in irpairs(target:orderules()) do -- reverse rules order for batchjobs:addjob()
        local script = r:script("build")
        if script then
            if r:extraconf("build", "batch") then
                job, job_leaf = assert(script(target, batchjobs,
                                              {rootjob = job or rootjob}),
                                       "rule(%s):on_build(): no returned job!",
                                       r:name())
            else
                job = batchjobs:addjob("rule/" .. r:name() .. "/build",
                                       function(index, total, opt)
                    script(target, {progress = opt.progress})
                end, {rootjob = job or rootjob})
            end
        else
            local buildcmd = r:script("buildcmd")
            if buildcmd then
                job = batchjobs:addjob("rule/" .. r:name() .. "/build",
                                       function(index, total, opt)
                    local batchcmds_ = batchcmds.new({target = target})
                    buildcmd(target, batchcmds_, {progress = opt.progress})
                    batchcmds_:runcmds({
                        changed = target:is_rebuilt(),
                        dryrun = option.get("dry-run")
                    })
                end, {rootjob = job or rootjob})
            end
        end
    end

    -- uses the builtin target script
    if not job and
        (target:is_static() or target:is_binary() or target:is_shared() or
            target:is_object() or target:is_moduleonly()) then
        job, job_leaf = import("kinds." .. target:kind(), {anonymous = true})(
                            batchjobs, rootjob, target)
    end
    job = job or rootjob
    return job, job_leaf or job
end

-- add batch jobs
function _add_batchjobs(batchjobs, rootjob, target)

    local job, job_leaf
    local script = target:script("build")
    if not script then
        -- do builtin batch jobs
        job, job_leaf = _add_batchjobs_builtin(batchjobs, rootjob, target)
    elseif target:extraconf("build", "batch") then
        -- do custom batch script
        -- e.g.
        -- target("test")
        --     on_build(function (target, batchjobs, opt)
        --         return batchjobs:addjob("test", function (idx, total)
        --             print("build it")
        --         end, {rootjob = opt.rootjob})
        --     end, {batch = true})
        --
        job, job_leaf = assert(script(target, batchjobs, {rootjob = rootjob}),
                               "target(%s):on_build(): no returned job!",
                               target:name())
    else
        -- do custom script directly
        -- e.g.
        --
        -- target("test")
        --     on_build(function (target, opt)
        --         print("build it")
        --     end)
        --
        job = batchjobs:addjob(target:name() .. "/build",
                               function(index, total, opt)
            script(target, {progress = opt.progress})
        end, {rootjob = rootjob})
    end
    return job, job_leaf or job
end

-- add batch jobs for the given target
function _add_batchjobs_for_target(batchjobs, rootjob, target)

    -- has been disabled?
    if not target:is_enabled() then
        return
    end

    -- add after_build job for target
    local pkgenvs = _g.pkgenvs or {}
    _g.pkgenvs = pkgenvs
    local job_build_after = batchjobs:addjob(target:name() .. "/after_build",
                                             function(index, total, opt)

        -- do after_build
        local progress = opt.progress
        local after_build = target:script("build_after")
        if after_build then
            after_build(target, {progress = progress})
        end
        for _, r in ipairs(target:orderules()) do
            local after_build = r:script("build_after")
            if after_build then
                after_build(target, {progress = progress})
            else
                local after_buildcmd = r:script("buildcmd_after")
                if after_buildcmd then
                    local batchcmds_ = batchcmds.new({target = target})
                    after_buildcmd(target, batchcmds_, {progress = progress})
                    batchcmds_:runcmds({
                        changed = target:is_rebuilt(),
                        dryrun = option.get("dry-run")
                    })
                end
            end
        end

        -- restore environments
        if target:pkgenvs() then
            pkgenvs.oldenvs = pkgenvs.oldenvs or os.getenvs()
            pkgenvs.newenvs = pkgenvs.newenvs or {}
            pkgenvs.newenvs[target] = nil
            local newenvs = pkgenvs.oldenvs
            for _, envs in pairs(pkgenvs.newenvs) do
                newenvs = os.joinenvs(envs, newenvs)
            end
            os.setenvs(newenvs)
        end

    end, {
        rootjob = rootjob,
        high_priority = target:policy("build.high_priority")
    })

    -- add batch jobs for target, @note only on_build script support batch jobs
    local job_build, job_build_leaf = _add_batchjobs(batchjobs, job_build_after,
                                                     target)

    -- add before_build job for target
    local job_build_before = batchjobs:addjob(target:name() .. "/before_build",
                                              function(index, total, opt)

        -- enter package environments
        -- https://github.com/xmake-io/xmake/issues/4033
        --
        -- maybe mixing envs isn't a great solution,
        -- but it's the most efficient compromise compared to setting envs in every on_build_file.
        --
        if target:pkgenvs() then
            pkgenvs.oldenvs = pkgenvs.oldenvs or os.getenvs()
            pkgenvs.newenvs = pkgenvs.newenvs or {}
            pkgenvs.newenvs[target] = target:pkgenvs()
            local newenvs = pkgenvs.oldenvs
            for _, envs in pairs(pkgenvs.newenvs) do
                newenvs = os.joinenvs(envs, newenvs)
            end
            os.setenvs(newenvs)
        end

        -- clean target if rebuild
        if target:is_rebuilt() and not option.get("dry-run") then
            _clean_target(target)
        end

        -- do before_build
        -- we cannot add batchjobs for this rule scripts, @see https://github.com/xmake-io/xmake/issues/2684
        local progress = opt.progress
        local before_build = target:script("build_before")
        if before_build then
            before_build(target, {progress = progress})
        end
        for _, r in ipairs(target:orderules()) do
            local before_build = r:script("build_before")
            if before_build then
                before_build(target, {progress = progress})
            else
                local before_buildcmd = r:script("buildcmd_before")
                if before_buildcmd then
                    local batchcmds_ = batchcmds.new({target = target})
                    before_buildcmd(target, batchcmds_, {progress = progress})
                    batchcmds_:runcmds({
                        changed = target:is_rebuilt(),
                        dryrun = option.get("dry-run")
                    })
                end
            end
        end
    end, {
        rootjob = job_build_leaf,
        high_priority = target:policy("build.high_priority")
    })
    return job_build_before, job_build, job_build_after
end

-- add batch jobs for the given target and deps
-- 为给定的 target 和其依赖构造依赖图, 存储到 batchjobs 中
function _add_batchjobs_for_target_and_deps(batchjobs, rootjob, target,
                                            all_target_jobs)
    -- 存储已经处理过的 target, 避免重复处理
    local targetjob_ref = all_target_jobs.after_build[target:name()]
    if targetjob_ref then
        batchjobs:add(targetjob_ref, rootjob)
    else
        local job_build_before, job_build, job_build_after =
            _add_batchjobs_for_target(batchjobs, rootjob, target)
        if job_build_before and job_build and job_build_after then
            all_target_jobs.build_root[target:name()] = job_build
            all_target_jobs.after_build[target:name()] = job_build_after
            all_target_jobs.before_build[target:name()] = job_build_before

            -- 注意这里只会处理直接依赖, 所以对于一些间接依赖我们后面需要打补丁
            for _, depname in ipairs(target:get("deps")) do
                local dep = project.target(depname)
                local targetjob

                -- @see https://github.com/xmake-io/xmake/discussions/2500
                if dep:policy("build.across_targets_in_parallel") == false then
                    targetjob = job_build_before
                else
                    -- fully parallel compilation for object target and it's deps, make it easy to set `build.high_priority`
                    targetjob = rootjob
                end
                _add_batchjobs_for_target_and_deps(batchjobs, targetjob, dep,
                                                   all_target_jobs)
            end
        end
    end
end

-- add batch jobs for non-object target to make sure it's build job will wait for all it's direct
-- and indirect deps' `after_build` jobs are compiled
-- 确保 non-object target 的 `before_build` 任务依赖它的所有直接和间接 target 的 `after_build` 任务
function _add_batchjobs_for_non_object_target(batchjobs, target, root_job,
                                              build_after_jobs, visited_deps)
    if visited_deps[target] then
        return
    end
    visited_deps[target] = true
    for _, dep in ipairs(target:orderdeps()) do
        local dep_job_after = build_after_jobs[dep:name()]
        if dep_job_after then
            batchjobs:add(dep_job_after, root_job)
            _add_batchjobs_for_non_object_target(batchjobs, dep, root_job,
                                                 build_after_jobs, visited_deps)
        end
    end
end

-- get batch jobs, @note we need to export it for private.diagnosis.dump_buildjobs
--
-- 可以通过 xmake l private.diagnosis.dump_buildjobs [targetname] 命令来查看编译依赖图
function get_batchjobs(targetnames, group_pattern)

    -- get root targets (not depended on by any other target)
    -- 获取 root target (不被任何其他 target 依赖的 target)
    local targets_root = {}
    if targetnames then
        -- 如果指定了 targetnames, 则将这个 targetnames 全部添加到 targets_root 中
        for _, targetname in ipairs(table.wrap(targetnames)) do
            local target = project.target(targetname)
            if target then
                table.insert(targets_root, target)
                if option.get("rebuild") then
                    target:data_set("rebuilt", true)
                    if not option.get("shallow") then
                        for _, dep in ipairs(target:orderdeps()) do
                            dep:data_set("rebuilt", true)
                        end
                    end
                end
            end
        end
    else
        -- 所有被依赖的 target 集合
        local depset = hashset.new()
        -- 所有编译目标的 target 列表
        local targets = {}
        for _, target in ipairs(project.ordertargets()) do
            if target:is_enabled() then
                local group = target:get("group")
                if (target:is_default() and not group_pattern) or
                    option.get("all") or
                    (group_pattern and group and group:match(group_pattern)) then
                    for _, depname in ipairs(target:get("deps")) do
                        depset:insert(depname)
                    end
                    table.insert(targets, target)
                end
            end
        end
        for _, target in ipairs(targets) do
            -- 没有被任何其他 target 依赖的 target 入队 targets_root
            if not depset:has(target:name()) then
                table.insert(targets_root, target)
            end
            if option.get("rebuild") then
                target:data_set("rebuilt", true)
            end
        end
    end

    -- contains several tables, each table's key is target_name and the value is a job function
    -- 编译任务表， key 是 target 名, value 对应的编译任务
    local all_target_jobs = {
        after_build = {}, -- each target's `after_build` job
        before_build = {}, -- each target's `before_build` job
        build_root = {} -- each target's build-root job, it may be `link` job in most cases
    }

    -- generate batch jobs for default or all targets
    -- 遍历所有的 targets_root, 准备生成编译依赖图 batchjobs
    local batchjobs = jobpool.new()
    for _, target in ipairs(targets_root) do
        _add_batchjobs_for_target_and_deps(batchjobs, batchjobs:rootjob(),
                                           target, all_target_jobs)
    end

    -- make sure static/shared/binary target's `build_root` job waits until all of it's direct and indirect
    -- dependent `after_build` tasks are compiled
    --
    -- non-object target 的依赖补丁
    -- 由于我们让 target 尽可能并行, 但是 non-object target 是需要等待所有直接依赖和间接依赖 target 的 .o 文件编译完成才能启动的
    -- 所以我们让 non-object target 的 `build_root` 任务等待所有直接依赖和间接依赖 target 的 `after_build` 任务完成
    for _, target in ipairs(project.ordertargets()) do
        local target_kind = target:kind()
        local non_object_kind = target_kind == "static" or target_kind ==
                                    "shared" or target_kind == "binary"
        local visited_deps = {}
        if non_object_kind then
            local root_job = all_target_jobs.build_root[target:name()]
            if root_job then
                _add_batchjobs_for_non_object_target(batchjobs, target,
                                                     root_job,
                                                     all_target_jobs.after_build,
                                                     visited_deps)
            end
        end
    end

    -- object target 的依赖补丁
    -- 由于我们给所有 object target 都默认生成动态库来提前检查缺失的符号, 考虑这么一个例子 (example/policy/build.across_targets_in_parallel)
    -- object target B 依赖 static target C, 由于 B 需要生成 libB.so 来检查缺失符号, 那么 libB.so 必须要等待 libC.a 编译完成
    for _, target in ipairs(project.ordertargets()) do
        local target_kind = target:kind()
        local target_root_job = all_target_jobs.build_root[target:name()]

        if target_kind == "object" then
            -- 遍历 object target 的所有直接依赖和间接依赖, 如果是 static / shared / binary 则需要等其 build_root 任务结束才能开始编译
            for _, dep in ipairs(target:orderdeps({inherit = true})) do
                local dep_kind = dep:kind()
                if dep_kind == "static"  or dep_kind == "shared" or dep_kind == "binary" then
                    local dep_root_job = all_target_jobs.build_root[dep:name()]
                    if dep_root_job then
                        batchjobs:add(dep_root_job, target_root_job)
                    end
                end
            end
        end
    end

    -- add fence jobs, @see https://github.com/xmake-io/xmake/issues/5003
    for _, target in ipairs(project.ordertargets()) do
        local target_job_before = all_target_jobs.before_build[target:name()]
        if target_job_before then
            for _, dep in ipairs(target:orderdeps()) do
                if dep:policy("build.fence") then
                    local fence_job = all_target_jobs.after_build[dep:name()]
                    if fence_job then
                        batchjobs:add(fence_job, target_job_before)
                    end
                end
            end
        end
    end

    return batchjobs
end

-- the main entry
function main(targetnames, group_pattern)

    -- enable distcc?
    local distcc
    if distcc_build_client.is_connected() then
        distcc = distcc_build_client.singleton()
    end

    -- build all jobs
    local t_start = os.mclock()
    local batchjobs = get_batchjobs(targetnames, group_pattern)
    local t_cost = os.mclock() - t_start
    if option.get("verbose") or option.get("diagnosis") then
        utils.cprint(
            "${bright blue}[improvement]${clear} construct batchjobs graph cost [" ..
                tostring(t_cost / 1000) .. "s]")
        io.writefile(path.join(config.debugdir(), "build-batchjobs.txt"),
                     tostring(batchjobs))
    end

    -- prune redundant edges
    -- t_start = os.mclock()
    -- batchjobs:prune_redundant_edges()
    -- t_cost = os.mclock() - t_start
    -- if option.get("verbose") or option.get("diagnosis") then
    --     utils.cprint(
    --         "${bright blue}[improvement]${clear} prune redundant edges cost [" ..
    --             tostring(t_cost / 1000) .. "s]")
    -- end

    -- run batchjobs graph
    t_start = os.mclock()
    if batchjobs and batchjobs:size() > 0 then
        local curdir = os.curdir()
        runbatchjobs("build", batchjobs, {
            on_exit = function(errors)
                import("utils.progress")
                if errors and progress.showing_without_scroll() then
                    print("")
                end
            end,
            comax = option.get("jobs") or 1,
            curdir = curdir,
            distcc = distcc
        })
        os.cd(curdir)
    end
    t_cost = os.mclock() - t_start
    if option.get("verbose") or option.get("diagnosis") then
        utils.cprint(
            "${bright blue}[improvement]${clear} run batchjobs graph cost [" ..
                tostring(t_cost / 1000) .. "s]")
    end
end
