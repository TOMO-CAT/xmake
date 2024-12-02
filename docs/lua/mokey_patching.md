# Monkey Patching

## 简介

Monkey Patching 是一种编程技术，指在运行时动态修改或扩展现有模块、类、或函数的行为，而无需修改原始代码文件。

在 Lua 的上下文中，它通常用于：

* **增强功能**：在调用原始函数之前或之后增加逻辑
* **修复问题**：修补（patch）现有模块中的缺陷
* **自定义行为**：通过覆盖标准 API 实现特定项目的需求

## 例子

以 `xmake/core/base/coroutine.lua` 为例，它扩展了原有的 `coroutine.resume` 方法：

```lua
-- 保存原始的 coroutine.resume 方法, 可以我们可以在新的 resume 实现中调用原始的方法
coroutine._resume  = coroutine._resume or coroutine.resume

-- 自定义 coroutine.resume 方法
function coroutine.resume(co, ...)
    local ok, results = coroutine._resume(co, ...)
    ...
end
```
