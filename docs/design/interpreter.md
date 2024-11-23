# 解释器

## 结构体

```lua
-- new an interpreter instance
function interpreter.new()
    ...

    -- init an interpreter instance
    local instance = {
        _PUBLIC = {}
        ,
        _PRIVATE = {
            _SCOPES = {}
            ,
            _MTIMES = {}
            ,
            _SCRIPT_FILES = {}
            ,
            _FILTER = require("base/filter").new()
        }
    }
end
```

### 1. _PUBLIC

### 2. _PRIVATE

## filter && deduplicate 参数

主要作用在 `xmake/core/base/interpreter.lua` 中的 `interpreter:_handle` 函数中，用于在加载 scope info 时进行处理。

### 1. filter

### 2. deduplicate

首先有个 `deduplication policy` 控制了不同 value 的去重方向，默认是从左到右。但是对于 linux 的 `ld` 链接器，它要求链接顺序是从右到左，所以需要设置 `deduplication policy` 为 `right`。

> <https://github.com/xmake-io/xmake/issues/1903>

剩下的逻辑就是按照设置好的方向对数据进行去重。

另外需要注意的是去重的时候可以传入一个 barrier，影响去重逻辑。
