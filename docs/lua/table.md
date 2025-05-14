# table

## 排序

### 1. 基于 value 排序

### 2. 基于 key 排序

```lua
local data = {
    fig = 5,
    cherry = 7,
    apple = 10,
    date = 12,
    banana = 15,
}

for k, v in table.orderpairs(data) do
    print(k, v)
    -- output:
    --
    -- apple 10
    -- banana 15
    -- cherry 7
    -- date 12
    -- fig 5
end

print(table.orderkeys(data))
-- output:
--
-- {
--   "apple",
--   "banana",
--   "cherry",
--   "date",
--   "fig"
-- }
```

## setmetatable

### 1. `__index` 元方法

`__index` 用于当访问表中不存在的键时被调用。

### 2. `__newindex` 元方法

`__newindex` 用于当对表中不存在的键赋值时被调用。
