-- 示例：__newindex 控制表的写入行为
local restricted = {}
setmetatable(restricted, {
    __newindex = function(tbl, key, value)
        if key == "age" and type(value) ~= "number" then
            error("Age must be a number!")
        end
        rawset(tbl, key, value) -- 使用 rawset 跳过元表直接赋值
    end
})

restricted.name = "Alice"
print("Name:", restricted.name) -- 输出: Name: Alice

-- 尝试赋一个非法的值
-- restricted.age = "Twenty"  -- 会报错: Age must be a number!
restricted.age = 25
print("Age:", restricted.age) -- 输出: Age: 25
