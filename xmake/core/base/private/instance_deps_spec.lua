local instance_deps = instance_deps or {}
local table = table or {}

-- slice table array
function table.slice(self, first, last, step)
    local sliced = {}
    for i = first or 1, last or #self, step or 1 do
        sliced[#sliced + 1] = self[i]
    end
    return sliced
end

-- join all objects and tables
function table.join(...)
    local result = {}
    for _, t in ipairs({ ... }) do
        if type(t) == "table" and not t.__wrap_locked__ then
            for k, v in pairs(t) do
                if type(k) == "number" then
                    table.insert(result, v)
                else
                    result[k] = v
                end
            end
        else
            table.insert(result, t)
        end
    end
    return result
end

-- wrap value to array
function table.wrap(value)
    if nil == value then
        return {}
    end
    if type(value) ~= "table" or value.__wrap_locked__ then
        return { value }
    end
    return value
end

-- get keys of a table
function table.keys(tbl)
    local keyset = {}
    local n = 0
    for k, _ in pairs(tbl) do
        n = n + 1
        keyset[n] = k
    end
    return keyset, n
end

-- Sorts the keys of a given table and returns a list of the sorted keys.
-- The function supports a custom sorting rule via a callback function, or it can use the default sorting behavior.
--
-- Parameters:
--   - tbl (table): The table whose keys will be sorted.
--   - callback (function, optional): A custom comparison function for sorting the keys.
--                                    If not provided or not a function, default sorting is used.
--
-- Returns:
--   - keys (table): A list of the sorted keys from the table.
--
-- Notes:
--   - If the table contains keys of mixed types (e.g., numbers and strings),
--     the default sorting may raise an error. In such cases, the function will fall back to sorting
--     by converting the keys to strings.
--
-- Examples:
--   local t = {3 = "c", 1 = "a", 2 = "b"}
--   local sorted_keys = table.orderkeys(t)
--   -- sorted_keys is {1, 2, 3}

--   -- Sorting keys in descending order using a custom comparator:
--   local sorted_keys_desc = table.orderkeys(t, function(a, b) return a > b end)
--   -- sorted_keys_desc is {3, 2, 1}
function table.orderkeys(tbl, callback)
    local callback = type(callback) == "function" and callback or nil
    local keys = table.keys(tbl)
    if callback then
        table.sort(keys, callback)
    else
        local ok = pcall(table.sort, keys)
        if not ok then
            -- maybe sort strings and numbers, {1, 2, "a"}
            table.sort(keys, function(a, b) return tostring(a) < tostring(b) end)
        end
    end
    return keys
end

-- order key/value iterator
--
-- for k, v in table.orderpairs(t) do
--   TODO
-- end
function table.orderpairs(t, callback)
    if type(t) ~= "table" then
        t = t ~= nil and { t } or {}
    end
    local orderkeys = table.orderkeys(t, callback)
    local i = 1
    return function(t, k)
        k = orderkeys[i]
        i = i + 1
        return k, t[k]
    end, t, nil
end

-- sort the given instance with deps
function instance_deps._sort_instance(instance, instances, orderinstances, instancerefs, depspath)
    if not instancerefs[instance:name()] then
        instancerefs[instance:name()] = true
        for _, depname in ipairs(table.wrap(instance:get("deps"))) do
            local depinst = instances[depname]
            if depinst then
                local depspath_sub
                if depspath then
                    for idx, name in ipairs(depspath) do
                        if name == depname then
                            local circular_deps = table.slice(depspath, idx)
                            table.insert(circular_deps, depname)
                            error("circular dependency(" .. table.concat(circular_deps, ", ") .. ") detected!")
                        end
                    end
                    depspath_sub = table.join(depspath, depname)
                end
                instance_deps._sort_instance(depinst, instances, orderinstances, instancerefs, depspath_sub)
            end
        end
        table.insert(orderinstances, instance)
    end
end

-- sort instances with deps
--
-- e.g.
--
-- a.deps = b
-- b.deps = c
-- foo.deps = a d
--
-- orderdeps: c -> b -> a -> d -> foo
function instance_deps.sort(instances)
    local refs = {}
    local orderinstances = {}
    for _, instance in table.orderpairs(instances) do
        instance_deps._sort_instance(instance, instances, orderinstances, refs, { instance:name() })
    end
    return orderinstances
end

local function create_instance(name, deps)
    return {
        name = function() return name end,
        get = function(_, key)
            if key == "deps" then
                return deps
            end
            return nil
        end
    }
end

local function orderinstances_to_string(orderinstances)
    local sorted_names = {}
    for _, instance in ipairs(orderinstances) do
        table.insert(sorted_names, instance:name())
    end
    return table.concat(sorted_names, ", ")
end

describe("instance_deps.sort", function()
    it("sorts a simple dependency chain in correct order", function()
        -- Setup: a -> b -> c
        local instances = {
            a = create_instance("a", { "b" }),
            b = create_instance("b", { "c" }),
            c = create_instance("c", {})
        }
        local orderinstances = instance_deps.sort(instances)
        assert.are.equal("c, b, a", orderinstances_to_string(orderinstances))
    end)

    it("sorts multiple instances with independent chains", function()
        -- Setup: a -> b -> c, d -> e
        local instances = {
            a = create_instance("a", { "b" }),
            b = create_instance("b", { "c" }),
            c = create_instance("c", {}),
            d = create_instance("d", { "e" }),
            e = create_instance("e", {})
        }

        local orderinstances = instance_deps.sort(instances)
        -- split to two chains
        -- `c` should appear before `b` and `a`; `e` should appear before `d`
        assert.are.equal("c, b, a, e, d", orderinstances_to_string(orderinstances))
    end)

    it("handles multiple dependencies in correct order", function()
        -- Setup: foo -> a, d; a -> b; b -> c
        local instances = {
            foo = create_instance("foo", { "a", "d" }),
            a = create_instance("a", { "b" }),
            b = create_instance("b", { "c" }),
            c = create_instance("c", {}),
            d = create_instance("d", {})
        }

        local orderinstances = instance_deps.sort(instances)
        assert.are.equal("c, b, a, d, foo", orderinstances_to_string(orderinstances))
    end)

    it("sorts instances with no dependencies in lexicographical order", function()
        -- Setup: instances with no dependencies
        local instances = {
            b = create_instance("b", {}),
            a = create_instance("a", {}),
            d = create_instance("d", {}),
            c = create_instance("c", {}),
        }

        local orderinstances = instance_deps.sort(instances)
        assert.are.equal("a, b, c, d", orderinstances_to_string(orderinstances))
    end)

    it("sorts instances with circular dependencies", function()
        -- Setup: a -> b -> c -> a
        local instances = {
            a = create_instance("a", { "b" }),
            b = create_instance("b", { "c" }),
            c = create_instance("c", { "a" })
        }

        assert.has_error(function()
            instance_deps.sort(instances)
        end, "circular dependency(a, b, c, a) detected!")
    end)

    it("sorts with complex interdependent chains correctly", function()
        -- Setup: a -> b, b -> c, d -> b
        local instances = {
            a = create_instance("a", {"b"}),
            b = create_instance("b", {"c"}),
            c = create_instance("c", {}),
            d = create_instance("d", {"b"})
        }

        local orderinstances = instance_deps.sort(instances)
        -- `c` should be before `b`, `b` should be before both `a` and `d`
        assert.are.equal("c, b, a, d", orderinstances_to_string(orderinstances))
    end)
end)
