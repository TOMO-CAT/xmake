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
-- @file        jobpool.lua
--

-- imports
import("core.base.object")
import("core.base.list")
import("core.base.hashset")
import("core.base.option")

-- define module
local jobpool = jobpool or object {_init = {"_size", "_rootjob", "_leafjobs"}}

-- the job status
local JOB_STATUS_FREE     = 1
local JOB_STATUS_PENDING  = 2
local JOB_STATUS_FINISHED = 3

-- get jobs size
function jobpool:size()
    return self._size
end

-- get root job
function jobpool:rootjob()
    return self._rootjob
end

-- new run job
--
-- e.g.
-- local job = jobpool:newjob("xxx", function (index, total) end)
-- jobpool:add(job, rootjob1)
-- jobpool:add(job, rootjob2)
-- jobpool:add(job, rootjob3)
--
function jobpool:newjob(name, run, opt)
    opt = opt or {}
    return {name = name, run = run, distcc = opt.distcc, status = JOB_STATUS_FREE}
end

-- add run job to the given job node
--
-- e.g.
-- local job = jobpool:addjob("xxx", function (index, total) end, {rootjob = rootjob})
--
-- @param name      the job name
-- @param run       the run command/script
-- @param opt       the options (rootjob, distcc)
--                  we can support distcc build if distcc is true
--
function jobpool:addjob(name, run, opt)
    opt = opt or {}
    return self:add({name = name, run = run, distcc = opt.distcc, status = JOB_STATUS_FREE, high_priority = opt.high_priority}, opt.rootjob)
end

-- add job to the given job node
--
-- @param job       the job
-- @param rootjob   the root job node (optional)
--
function jobpool:add(job, rootjob)

    -- add job to the root job
    rootjob = rootjob or self:rootjob()
    rootjob._deps = rootjob._deps or hashset.new()
    rootjob._deps:insert(job)

    -- -- store all direct dependencies
    -- self._direct_dependencies = self._direct_dependencies or {}
    -- self._direct_dependencies[rootjob] = self._direct_dependencies[rootjob] or {}
    -- table.insert(self._direct_dependencies[rootjob], job)



    -- attach parents node
    local parents = job._parents
    if not parents then
        parents = {}
        job._parents = parents
        self._size = self._size + 1 -- @note only update number for new job without parents
    end
    table.insert(parents, rootjob)

    -- in group? attach the group node
    local group = self._group
    if group then
        job._deps = job._deps or hashset.new()
        job._deps:insert(group)
        group._parents = group._parents or {}
        table.insert(group._parents, job)
    end
    return job
end

-- get a free job from the leaf jobs
function jobpool:getfree()
    if self:size() == 0 then
        return
    end

    -- get a free job from the leaf jobs
    local leafjobs = self:_getleafjobs()
    if not leafjobs:empty() then
        -- try to get next free job fastly
        if self._nextfree then
            local job = self._nextfree
            local nextfree = leafjobs:prev(job)
            if nextfree ~= job and self:_isfree(nextfree) then
                self._nextfree = nextfree
            else
                self._nextfree = nil
            end
            job.status = JOB_STATUS_PENDING
            return job
        end
        -- find the next free job
        local removed_jobs = {}
        for job in leafjobs:ritems() do
            if self:_isfree(job) then
                local nextfree = leafjobs:prev(job)
                if nextfree ~= job and self:_isfree(nextfree) then
                    self._nextfree = nextfree
                end
                job.status = JOB_STATUS_PENDING
                return job
            elseif job.group or job.status == JOB_STATUS_FINISHED then
                table.insert(removed_jobs, job)
            end
        end
        -- not found? if remove group and referenced node exist,
        -- we try to remove them and find the next free job again
        if #removed_jobs > 0 then
            for _, job in ipairs(removed_jobs) do
                self:remove(job)
            end
            for job in leafjobs:ritems() do
                if self:_isfree(job) then
                    local nextfree = leafjobs:prev(job)
                    if nextfree ~= job and self:_isfree(nextfree) then
                        self._nextfree = nextfree
                    end
                    job.status = JOB_STATUS_PENDING
                    return job
                end
            end
        end
    end
end

-- remove the given job from the leaf jobs
function jobpool:remove(job)
    assert(self:size() > 0)
    local leafjobs = self:_getleafjobs()
    if not leafjobs:empty() then
        assert(job ~= self._nextfree)

        -- remove this job from leaf jobs
        job.status = JOB_STATUS_FINISHED
        leafjobs:remove(job)

        -- get parents node
        local parents = assert(job._parents, "invalid job without parents node!")

        -- update all parents nodes
        for _, p in ipairs(parents) do
            -- we need to avoid adding it to leafjobs repeatly, it will cause dead-loop when poping group job
            -- @see https://github.com/xmake-io/xmake/issues/2740
            if not p._leaf then
                p._deps:remove(job)
                if p._deps:empty() and self._size > 0 then
                    p._leaf = true

                    -- reorder leafjobs to advance high-priority targets as much as possible
                    -- the last element of the leafjobs with be executed first
                    if p.high_priority then
                        leafjobs:insert_last(p)
                        if option.get("verbose") and option.get("diagnosis") then
                            cprint("${bright blue}[improvement]${clear} insert high priority parent job [" .. p.name .. "]")
                        end
                    else
                        leafjobs:insert_first(p)
                    end
                end
            end
        end
    end
end

-- enter group
--
-- @param name      the group name
-- @param opt       the options, e.g. {rootjob = ..}
--
function jobpool:group_enter(name, opt)
    opt = opt or {}
    assert(not self._group, "jobpool: cannot enter group(%s)!", name)
    self._group = {name = name, group = true, rootjob = opt.rootjob}
end

-- leave group
--
-- @return          the group node
--
function jobpool:group_leave()
    local group = self._group
    self._group = nil
    if group then
        if group._parents then
            return group
        else
            -- we just return the rootjob if there is not any jobs in this group
            return group.rootjob
        end
    end
end

-- is free job?
-- we need to ignore group node (empty job) and referenced node (finished job)
function jobpool:_isfree(job)
    if job and job.status == JOB_STATUS_FREE and not job.group then
        return true
    end
end

-- get leaf jobs
function jobpool:_getleafjobs()
    local leafjobs = self._leafjobs
    if leafjobs == nil then
        leafjobs = list.new()
        local refs = {}
        self:_genleafjobs(self:rootjob(), leafjobs, refs)
        self._leafjobs = leafjobs
    end
    return leafjobs
end

-- generate all leaf jobs from the given job
function jobpool:_genleafjobs(job, leafjobs, refs)
    local deps = job._deps
    if deps and not deps:empty() then
        for _, dep in deps:keys() do
            local depkey = tostring(dep)
            if not refs[depkey] then
                refs[depkey] = true
                self:_genleafjobs(dep, leafjobs, refs)
            end
        end
    else
        job._leaf = true

        -- reorder leafjobs to advance high-priority targets as much as possible
        -- the last element of the leafjobs with be executed first
        if job.high_priority then
            leafjobs:insert_last(job)
            if option.get("verbose") and option.get("diagnosis") then
                cprint("${bright blue}[improvement]${clear} insert high priority job [" .. job.name .. "]")
            end
        else
            leafjobs:insert_first(job)
        end
    end
end

-- generate jobs tree for the given job
function jobpool:_gentree(job, refs)
    local tree = {job.group and ("group(" .. job.name .. ")") or job.name}
    local deps = job._deps
    if deps and not deps:empty() then
        for _, dep in deps:keys() do
            local depkey = tostring(dep)
            if refs[depkey] then
                local depname = dep.group and ("group(" .. dep.name .. ")") or dep.name
                table.insert(tree, "ref(" .. depname .. ")")
            else
                refs[depkey] = true
                table.insert(tree, self:_gentree(dep, refs))
            end
        end
    end
    -- strip tree
    local smalltree = hashset.new()
    for _, item in ipairs(tree) do
        item = table.unwrap(item)
        if smalltree:size() < 16 or type(item) == "table" then
            smalltree:insert(item)
        else
            smalltree:insert("...")
        end
    end
    return smalltree:to_array()
end

function jobpool:_gentree2(tree, job, refs)
    local jobkey = tostring(job)
    local jobname = job.group and ("group(" .. job.name .. ")") or job.name
    if refs[jobkey] then
        jobname = "ref(" .. jobname .. ")"
        tree[jobname] = {}
        return
    else
        refs[jobkey] = true
        tree[jobname] = {}
    end

    local deps = job._deps
    if deps and not deps:empty() then
        for _, dep in deps:keys() do
            self:_gentree2(tree[jobname], dep, refs)
        end
    end
end

function jobpool:prune_redundant_edges()

    local visited = {}
    local job_set = {}
    local function dfs(node, ancestors)
        if visited[node] then
            return
        end

        table.insert(job_set, node)
        -- node._parents = node._parents or {}
        node._ancestors = node._ancestors or {}
        -- table.insert(node._parents, parent)
        table.join2(node._ancestors, ancestors)
        local deps = node._deps
        if deps then
            for _, dep in deps:keys() do
                print("local process " .. dep.name)
                dfs(dep, table.join(node._parents, node._ancestors))
            end
        end
        visited[node] = true
    end

    dfs(self._rootjob, {})
    print("done")
    print("job set number: " .. #job_set)
    table.unique(job_set)
    print("job set number: " .. #job_set)

    for _, job in ipairs(job_set) do
        local direct_parents = table.unique(job._parents)
        local indirect_parents = table.unique(job._ancestors)
        for _, direct_p in ipairs(direct_parents) do
            if table.contains(indirect_parents, direct_p) then
                for idx, dep in ipairs(direct_p._deps) do
                    if dep == job then
                        table.remove(direct_p._deps, idx)
                    end
                end
                for idx, parent in ipairs(job._parents) do
                    if parent == direct_p then
                        table.remove(job._parents, idx)
                        print("delete " .. parent.name)
                    end
                end
                print(format("redudant edges [%s -> %s]", direct_p, job))
            end
        end
    end
end

-- tostring
function jobpool:__tostring()
    local build_time = os.mclock()

    self:prune_redundant_edges(job_set)
    build_time = os.mclock() - build_time
    print(format("prune redundant edges cost %ss", build_time / 1000))



    local job = self:getfree()
    for _, parents in ipairs(table.unique(job._parents)) do
        print(parents.name)
    end
    for _, ancestor in ipairs(table.unique(job._parents)) do
        print(ancestor.name)
    end
    print(job.name)

    -- raise("fck")

    local refs = {}

    -- return string.serialize(self:_gentree(self:rootjob(), refs), {indent = 2, orderkeys = true})

    local jobpool_tree = {}
    self:_gentree2(jobpool_tree, self:rootjob(), refs)
    -- print(#self._direct_dependencies)
    return import("core.base.json").encode(jobpool_tree)
end

-- new a jobpool
function new()
    return jobpool {0, {name = "root"}, nil}
end
