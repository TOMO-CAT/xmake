-- add envs for `xrepo env`
-- @see https://github.com/xmake-io/xmake/issues/5580
--
-- @code
-- includes("@builtin/xrepo")
--
-- xrepo_addenvs(function (package)
--     package:addenv("FOO", "FOO")
-- end)
--
-- xrepo_addenvs({BAR = "BAR"})
-- @endcode
--
function xrepo_addenvs(envs)
    local packagename = "__xrepo_addenvs_" .. (tostring(envs):gsub("%s", "_"))
    package(packagename)
        on_load(function (package)
            if type(envs) == "function" then
                envs(package)
            elseif type(envs) == "table" then
                for k, v in pairs(envs) do
                    package:addenv(k, v)
                end
            end
        end)
        on_fetch(function (package, opt)
            return {}
        end)
    package_end()
    add_requires(packagename)
end

-- add env for `xrepo env`
-- @see https://github.com/xmake-io/xmake/issues/5580
--
-- @code
-- includes("@builtin/xrepo")
--
-- xrepo_addenv("ZOO", ...)
-- @endcode
--
function xrepo_addenv(name, ...)
    local args = table.pack(...)
    local packagename = "xrepo_addenv_" .. name .. (table.concat(args):gsub("%s", "_"))
    package(packagename)
        on_load(function (package)
            package:addenv(name, table.unpack(args))
        end)
        on_fetch(function (package, opt)
            return {}
        end)
    package_end()
    add_requires(packagename)
end
