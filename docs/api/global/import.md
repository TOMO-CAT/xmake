# import

## 功能

import 用于导入 xmake 的扩展类库以及一些自定义的类库模块：

1. 优先从当前脚本目录下导入
2. 再从扩展类库中导入

## 从当前目录导入

导入的语法规则是基于 `.` 的类库路径规则，例如：

```lua
import("core.base.option")
import("core.base.task")

function main()

    -- 获取参数选项
    print(option.get("version"))

    -- 运行任务和插件
    task.run("hello")
end
```

假设我们的目录结构如下：

```lua
plugin
├── main.lua
├── modules
│   ├── hello1.lua
│   └── hello2.lua
└── xmake.lua
```

在 main.lua 中导入 modules：

```lua
import("modules.hello1")
import("modules.hello2")
```

导入后就可以直接使用里面的所有公有接口，**私有接口用 `_` 前缀标示，表明不会被导出**，不会被外部调用到。

## 从上层目录导入

假如我们要调用外层的模块，那么可以使用 `.` 来访问，假设我们的目录结构如下：

```lua
test/
├── foo
│   ├── bar
│   │   ├── bar.lua
│   │   └── main.lua
│   └── foo.lua
└── test.lua
```

那么 main.lua 引用模块可以这么写：

```lua
import("bar")
import(".foo")
import("..test")
```

## 从指定目录导入

除了当前目录，我们还可以导入其他指定目录里面的类库，例如：

```lua
import("hello3", {rootdir = "/home/xxx/modules"})
```

## 指定别名

为了防止命名冲突，导入后还可以指定的别名：

```lua
import("core.platform.platform", {alias = "p"})

function main()

    -- 这样我们就可以使用p来调用platform模块的plats接口，获取所有xmake支持的平台列表了
    utils.dump(p.plats())
end
```

## 作为继承导入

import 不仅可以导入类库，还支持导入的同时作为继承导入，实现模块间的继承关系：

```lua
import("xxx.xxx", {inherit = true})
```

这样导入的不是这个模块的引用，而是导入的这个模块的所有公有接口本身，这样就会跟当前模块的接口进行合并，实现模块间的继承。

## 其他特性

```lua
-- try 表示导入的模块不存在时仅仅返回 nil, 不会抛异常后中断
-- anonymous 为 true 表示导入的模块不会引入当前作用域, 仅仅在 import 接口返回导入的对象引用
import("xxx.xxx", {try = true, anonymous = true})
```

## 自定义扩展模块

xmake 对模块的编写规范是有一套约定规则的，并没有沿用 lua 原生的 require 导入机制，并不需要在模块中使用 return 来全局返回它。

假如我们有一个模块文件 foo.lua，它的内容如下：

```lua
function _foo(a, b)
    return a + b
end

function add(a, b)
    _foo(a, b)
end

function main(a, b)
    add(a, b)
end
```

其中 main 是**可选的**入口函数，如果设置，模块 foo 可以直接被调用，例如：

```lua
import("foo")
foo(1, 2)

-- or

import("foo")(1, 2)
```

其他不带下划线的为 public 模块接口函数，例如 add。

```lua
import("foo")
foo.add(1, 2)
```

而里面带下划线前缀的 `_foo` 是私有函数，模块内部使用，不对外导出，所以在外面用户是不能够调用它的。
