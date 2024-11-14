# 编译 batchjobs 依赖图

## 相关 policy

### 1. build.fence

对于 proto target / codgen target 而言，它们会通过代码生成 *.h 头文件，而依赖它们的 target 启动编译需要这些头文件。假设 target A 依赖一个 proto target B，那么我们会在 proto target B 上将 `build.fence` 设置为 true，从而令 target A 的 `before_build` job 等待 proto target B 的 `link` job 运行完后再执行。

### 2. build.across_targets_in_parallel

设置了 `build.across_targets_in_parallel` 为 false 后，它会限制父 target 和它的所有依赖的子 target 的并行度，这意味对于父 target 为 root的子依赖图，假设任意一个其中的 target A 依赖 target B，那么 target A 的 `before_build` job 会等待 target B 的 `after_build` job 运行，导致 CPU 利用率较低。

## batchjobs graph 构建流程

以 `example/rules/protobuf.cpp/xmake.lua` 的 `foo.proto` target 为例。

### 1. "${target_name}/after_build"

首先添加 `foo.proto/after_build`，它的主要逻辑是：

* 先运行 target 的 `target:after_build` 脚本
* 再**按顺序**遍历所有的 rules，执行 `rule:after_build`，没有的话再执行 `rule:after_buildcmd`

### 2. "${target_name}/link"

对于 object target 而言是 "${target_name}/fakelink"，在这个例子中即 `foo.proto/fakelink`，会执行链接操作，但是 object target 是空的。

### 3. build sourcebatch groups

调用 `rule_groups.build_sourcebatch_groups(target, sourcebatches)` 构造 sourcebatch groups。这一步主要是为了处理一些有依赖关系的 rule，group 深度代表依赖树的深度，在我们这个场景没有任何需要依赖的 rule 信息，所以 depth 为 1。

> <https://github.com/xmake-io/xmake/issues/2814>

groups 大致的格式如下：

```json
[
    {
        "protobuf.cpp" : {
            "rule": "table()",
            "target": "table()",
            "sourcebatch": {
                "rulename": "protobuf.cpp",
                "sourcefiles": [
                    "foo/proto/header.proto"
                ],
            }
        }
    }
]
```

处理完后要 reverse groups 保证最终的运行顺序。

### 4. add batchjobs for groups

遍历所有的 groups，添加如下 batchjobs：

* "${target_name}/after_build_files${group_idx}"
* "${target_name}/build_files${group_idx}"
* "${target_name}/before_build_files${group_idx}"

对于我们这个具体的 target 它会先添加 `foo.proto/build_files1` group，具体逻辑是遍历所有的 proto 文件，添加一个以 proto 文件名命名的 batchjob（此场景是 `foo/proto/header.proto`）：

```lua
-- add batch jobs for the custom rule
function _add_batchjobs_for_rule(batchjobs, rootjob, target, sourcebatch, suffix)

    ...

    -- add batch jobs for xx_buildcmd_file
    if not script then
        scriptname = "buildcmd_file" .. (suffix and ("_" .. suffix) or "")
        script = ruleinst:script(scriptname)
        if script then
            local sourcekind = sourcebatch.sourcekind
            for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                batchjobs:addjob(sourcefile, function (index, total, opt)
                    local batchcmds_ = batchcmds.new({target = target})
                    script(target, batchcmds_, sourcefile, {sourcekind = sourcekind, progress = opt.progress})
                    batchcmds_:runcmds({changed = target:is_rebuilt(), dryrun = option.get("dry-run")})
                end, {rootjob = rootjob, distcc = ruleinst:extraconf(scriptname, "distcc")})
            end
        end
    end

    ...

end
```

继续添加 `foo.proto/before_build_files1` group，由于 `protobuf.cpp` rule 有 `before_build_files` 脚本（而且是 batch 模式的），因此会执行调用 `before_build_file` 脚本来修改 batchjobs：

```lua
-- add batch jobs for the custom rule
function _add_batchjobs_for_rule(batchjobs, rootjob, target, sourcebatch, suffix)

    ...

    -- add batch jobs for xx_build_files
    local scriptname = "build_files" .. (suffix and ("_" .. suffix) or "")
    local script = ruleinst:script(scriptname)
    if script then
        if ruleinst:extraconf(scriptname, "batch") then
            script(target, batchjobs, sourcebatch, {rootjob = rootjob, distcc = ruleinst:extraconf(scriptname, "distcc")})
        else

            ...

        end
    end

    ...

end
```

定义在 `protobuf.cpp` rule：

```lua
rule("protobuf.cpp", function()

    ...

    before_build_files(function (target, batchjobs, sourcebatch, opt)
        import("proto").build_cxfiles(target, batchjobs, sourcebatch, opt, "cxx")
    end, {batch = true})

    ...

end)
```

而这个脚本会构造一个 nodes 侵入到 batchjobs 里，注意这些所有新增加的 job 都在 `foo.proto/before_build_files1` 里。

```lua
-- build batch jobs
function build_cxfiles(target, batchjobs, sourcebatch, opt, sourcekind)
    opt = opt or {}
    local nodes = {}
    local nodenames = {}
    local node_rulename = "rules/" .. sourcebatch.rulename .. "/node"
    local sourcefiles = sourcebatch.sourcefiles
    for _, sourcefile_proto in ipairs(sourcefiles) do
        local nodename = node_rulename .. "/" .. sourcefile_proto
        nodes[nodename] = {
            name = nodename,
            job = batchjobs:addjob(nodename, function(index, total, jobopt)
                local batchcmds_ = batchcmds.new({target = target})
                buildcmd_pfiles(target, batchcmds_, sourcefile_proto, {progress = jobopt.progress}, sourcekind)
                batchcmds_:runcmds({changed = target:is_rebuilt(), dryrun = option.get("dry-run")})
            end)
        }
        table.insert(nodenames, nodename)
    end
    local rootname = "rules/" .. sourcebatch.rulename .. "/root"
    nodes[rootname] = {
        name = rootname,
        deps = nodenames,
        job = batchjobs:addjob(rootname, function(_index, _total)
            build_cxfile_objects(target, batchjobs, opt, sourcekind)
        end)
    }
    buildjobs(nodes, batchjobs, opt.rootjob)
end
```

### 5. "${target_name}/before_build"

它的执行逻辑是：

* 先运行 `target:before_build` 脚本
* 遍历所有的 rule 先运行 `rule:before_build`，没有的话再运行 `rule:before_buildcmd`

对于 `protobuf.cpp` rule 而言这一块没什么逻辑。

## 细节

### 1. non-object target 的依赖补丁

由于我们让 target 尽可能地并行，即默认都令 `build.across_targets_in_parallel` 为 true，再通过 `build.fence` 做强制依赖。这时候可能会丢失 `build.fence` target 的间接依赖关系导致报错。

因此我们对于所有的 non-object-target，在构造完 batchjobs 后强制令它的 `before_build` 任务依赖它的所有直接和间接依赖的 target 的 `after_build` 任务。

```lua
-- make sure static/shared/binary target waits until all of it's direct and indirect
-- dependent `afrer_build` tasks are compiled
for _, target in ipairs(project.ordertargets()) do
    local target_kind = target:kind()
    local non_object_kind = target_kind == "static" or target_kind == "shared" or target_kind == "binary"
    local visited_deps = {}
    if non_object_kind then
        local root_job = build_jobs[target:name()]
        if root_job then
            _add_batchjobs_for_non_object_target(batchjobs, target, root_job, build_after_jobs, visited_deps)
        end
    end
end
```

> 后续这里补充一张图。
