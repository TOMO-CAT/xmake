# 沙盒模式

为了简化 xmake.lua 中自己定义脚本、插件脚本以及扩展模块的开发，提供更高的安全性，xmake 采用了沙盒的方式去加载他们，每个脚本都独立在一个单独的沙盒中，并且有独立的作用域支持。

在 xmake 的整个目录结构中，** 除了 `xmake/core` 底层核心模块 **，其他的顶层目录全部放置在沙盒中加载运行，来简化代码开发，提高易用性和安全性。

这样有什么好处呢？

* 上层模块的开发跟 xmake 的 core 代码完全隔离，不会干扰到 xmake 的底层核心模块，更加安全
* 作用域只提供当前模块所需 api 的最小子集支持
* 提供内置异常处理机制，以及 api 调用的返回值、传参有效性检测，代码出错后，会及时报出异常信息，并提供详细栈来定位问题
* 可通过 import("core.base.task") 的模块导入方式，提供更加方便高效的模块加载支持
* 通过异常机制，简化代码的编写，可以简化 30% 的代码量，以及错误判断
* 常用接口字符串参数，支持 `$(val)` 等内置变量的自动转换

## 导入和返回值判断的改进

原生代码：

```lua
local test = require("modules/test")

local ok, errors = test.run("arg1", "arg2")
if not ok then
    os.raise(errors)
end
```

沙盒代码：

```lua
import("modules.test")

test.run("arg1", "arg2")
```

如果 test.run 运行出错，会自动中断，并显示出错信息。

import 的导入比 lua 原生的 require 更加的强大易用，支持很多高级特性：

* 多级多目录导入支持，模块的搜索目录可以有多个，也可以在 xmake.lua 中自己指定
* 通过指定父目录，批量加载所有模块，例如：import("core") 会加载 core 目录下的所有模块，并且通过 core.xxx 来访问
* 支持模块继承导入
* 支持匿名导入和缓存优化
* 支持别名设置
* 通过下划线区分，仅导入公有接口
* 自动识别 main 函数，可直接调用，例如：`import("test")(args)`，会自动调用 test 模块中的 main 函数
* 支持 xmake lua 直接加载测试，例如：`xmake l lib.detect.find_package zlib`

## 内置 api 的改进

原生代码：

```lua
print("hello xmake")
print("hello", "xmake")
```

沙盒代码：

```lua
print("hello xmake")
print("hello", "xmake")
print("hello %s", "xmake")
print("hello $(var)")
```

## 空参数的判断改进

原生代码：

```lua
function test(array)
    if array ~= nil then
        for _, item in ipairs(array) do
            -- ...
        end
    end
end
```

沙盒代码：

```lua
function test(array)
    for _, item in ipairs(array) do
        -- ...
    end
end
```

## 简化模块定义

原生代码：

```lua
local module = module or {}

function module.test(arg)
    -- ...
end

return module
```

沙盒代码：

```lua
function test(arg)
    -- ...
end
```

## Reference

* [1] [xmake 源码架构剖析](https://tboox.org/cn/2017/09/28/xmake-sourcecode-arch/)
