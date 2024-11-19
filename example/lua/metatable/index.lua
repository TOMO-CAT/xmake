-- 示例：__index 提供默认值
local defaults = {
    name = "Unknown",
    age = 0
}

local person = {
}
setmetatable(person, {
    __index = function(tbl, key)
        -- 如果访问的键不存在，则返回默认值
        return defaults[key] or "Key not found"
    end
})

print("Name:", person.name)  -- 输出: Name: Unknown
print("Age:", person.age)    -- 输出: Age: 0
print("Gender:", person.gender) -- 输出: Gender: Key not found
