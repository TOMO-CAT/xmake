# 协程调度器

## _coroutine

基于 Lua 原生 coroutine 扩展出来的协程结构体，方便调度器进行调度。

* **_ISOLATED**：
* **_TIMER_TASK**：

## scheduler

### 1. 字段

```lua
-- scheduler 管理的所有 coroutine; key 是 coroutine:thread(), value 是 _coroutine
_CO_TASKS: map table

-- FIXME: 存储了每个 co-group 及其对应的 coroutine 任务; key 是 co-group-name, value 是 coroutine 列表
_CO_GROUPS: map table

-- FIXME: coroutine group
_CO_GROUPS_PENDING： map table
```

### 2. 方法

```lua
-- 返回所有正在 coroutine
function scheduler:co_tasks()

-- 获取正在运行的 coroutine
function scheduler:co_running()

-- 启动一个 coroutine group 并添加到 _CO_GROUPS_PENDING, 调用 scopefunc 完后从 _CO_GROUPS_PENDING 里移除
--
-- @scopefunc: 入参为 _CO_GROUPS[name]
function scheduler:co_group_begin(name, scopefunc)

-- 使用给定的 opt 启动一个新的 coroutine
--
-- 1. 添加到 self._CO_TASKS
-- 2. self._CO_COUNT +1
-- 3. 将 coroutine 添加到 self._CO_GROUPS_PENDING 的每个 group 中
function scheduler:co_start_withopt(opt, cotask, ...)
```

### 3. 例子

以编译任务为例，假设我们有 972 个任务，并发数为 82，那么调用 scheduler 的流程如下：

```lua
local total = jobs:size()    -- jobs 是我们构建好的依赖图, size 为 972
local index = 0                   -- 当前运行的 job index
local comax = option.get("jobs")  -- 并发数, 这里为 82

while index < total do
    scheduler.co_group_begin("build", function (co_group)
        local freemax = comax -#co_group                     -- 当前最大的并发数
        local local_max = math.min(index + freemax, total)
        local total_max = local_max                          -- 当前可运行的最大的 job 索引 (取决于并发数和 total)

        -- 运行 job 索引为 [1, totl_max] 的任务
        while index < total_max do
            -- 从 batchjobs 中获取一个可运行的任务, 如果获取不到 (说明没有叶子节点可供运行, 都至少存在一个前置依赖未完成) 则退出循环
            local job = jobs:getfree()
            if not job then
                break
            end
            local jobfunc = job.run
            local jobname = job.name

            -- 运行这个任务
            index = index + 1
            scheduler.co_start_withopt({name = name .. '/' .. jobname, isolate = opt.isolate},
            function(i)
                try {
                    function()
                        jobfunc(i, total)  -- 调用任务
                    end,
                    catch {},
                    finally {
                        function()
                            if job then
                                jobs:remove(job)  -- 运行完毕后从依赖图中删除该任务
                            end
                        end
                    }
                }
            end, index)
        end
    end)
end
```

> 改进：启动 comax 对应的 coroutine，然后每个 coroutine 的逻辑都是不停从 batchjobs 中获取 freejob 运行，直到消费完整棵依赖树。
