-- imports
import("core.base.task")
import("core.base.option")

local params = {}

if option.get("quiet") then table.insert(params, "-q") end
if option.get("yes") then table.insert(params, "-y") end
if option.get("verbose") then table.insert(params, "-v") end
if option.get("diagnosis") then table.insert(params, "-D") end

function _run_test(script)
    assert(script:endswith("test.lua"))
    os.execv(os.programfile(), table.join("lua", params,
                                 path.join(os.scriptdir(), "runner.lua"), script))
end

-- run test with the given name
function _run_test_filter(name)
    local tests = {}
    local root = path.absolute(os.scriptdir())
    -- ${root}/tests/**/${name}/**/test.lua
    for _, script in ipairs(os.files(path.join(root, "**", name, "**",
                                               "test.lua"))) do
        if not script:find(".xmake", 1, true) then
            table.insert(tests, path.absolute(script))
        end
    end
    -- ${root}/tests/${name}/**/test.lua
    for _, script in ipairs(os.files(path.join(root, name, "**", "test.lua"))) do
        if not script:find(".xmake", 1, true) then
            table.insert(tests, path.absolute(script))
        end
    end
    -- ${root}/tests/**/${name}/test.lua
    for _, script in ipairs(os.files(path.join(root, "**", name, "test.lua"))) do
        if not script:find(".xmake", 1, true) then
            table.insert(tests, path.absolute(script))
        end
    end
    -- ${root}/${name}/test.lua
    for _, script in ipairs(os.files(path.join(root, name, "test.lua"))) do
        if not script:find(".xmake", 1, true) then
            table.insert(tests, path.absolute(script))
        end
    end

    tests = table.unique(tests)
    if #tests == 0 then
        utils.warning("no test have run, please check your filter .. (%s)", name)
    else
        cprint("> %d test(s) found", #tests)
        if option.get("diagnosis") then
            for _, v in ipairs(tests) do cprint(">     %s", v) end
        end

        for _, v in ipairs(tests) do _run_test(v) end
        cprint("> %d test(s) succeed", #tests)
    end
end

function main(name)
    if name then
        print('running tests for filter [' .. name .. ']')
    else
        print('running all tests')
    end
    return _run_test_filter(name or "/")
end
