--!A cross-platform build utility based on Lua
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--
-- Copyright (C) 2015-present, TBOOX Open Source Group.
--
-- @author      ruki
-- @file        main.lua
--

-- imports
import("core.base.option")
import("core.base.task")
import("core.project.config")
import("core.base.global")
import("core.project.project")
import("core.platform.platform")
import("core.theme.theme")
import("async.runjobs")
import("private.action.run.runenvs")
import("private.service.remote_build.action", {alias = "remote_build_action"})
import("actions.build.main", {rootdir = os.programdir(), alias = "build_action"})
import("utils.progress")

-- test target
function _do_test_target(target, opt)
    opt = opt or {}

    -- early out: results were computed during build
    if opt.build_should_fail or opt.build_should_pass then
        if opt.errors then
            vprint(opt.errors)
        end
        return opt.passed
    end

    -- get run environments
    local envs = opt.runenvs
    if not envs then
        local addenvs, setenvs = runenvs.make(target)
        envs = runenvs.join(addenvs, setenvs)
    end

    -- run test
    local outdata
    local errors
    local rundir = opt.rundir or target:rundir()
    local targetfile = path.absolute(target:targetfile())
    local runargs = table.wrap(opt.runargs or target:get("runargs"))
    local outfile = os.tmpfile()
    local errfile = os.tmpfile()
    local run_timeout = opt.run_timeout or 60000  -- set default run_timeout to 60 seconds
    local ok, syserrors = os.execv(targetfile, runargs, {try = true, timeout = run_timeout,
        curdir = rundir, envs = envs, stdout = outfile, stderr = errfile})
    local outdata = os.isfile(outfile) and io.readfile(outfile) or ""
    local errdata = os.isfile(errfile) and io.readfile(errfile) or ""
    if outdata and #outdata > 0 then
        opt.stdout = outdata
        vprint(outdata)
    end
    if errdata and #errdata > 0 then
        opt.stderr = errdata
        vprint(errdata)
    end
    if opt.trim_output then
        outdata = outdata:trim()
    end
    if ok ~= 0 then
        if not errors or #errors == 0 then
            if ok ~= nil then
                if syserrors then
                    errors = string.format("run %s failed, exit code: %d, exit error: %s", opt.name, ok, syserrors)
                else
                    errors = string.format("run %s failed, exit code: %d", opt.name, ok)
                end
            else
                errors = string.format("run %s failed, exit error: %s", opt.name, syserrors and syserrors or "unknown reason")
            end
        end
    end
    os.tryrm(outfile)
    os.tryrm(errfile)

    if ok == 0 then
        local passed
        local pass_outputs = table.wrap(opt.pass_outputs)
        local fail_outputs = table.wrap(opt.fail_outputs)
        for _, pass_output in ipairs(pass_outputs) do
            if opt.plain then
                if pass_output == outdata then
                    passed = true
                    break
                end
            else
                if outdata:match("^" .. pass_output .. "$") then
                    passed = true
                    break
                end
            end
        end
        for _, fail_output in ipairs(fail_outputs) do
            if opt.plain then
                if fail_output == outdata then
                    passed = false
                    if not errors or #errors == 0 then
                        errors = string.format("matched failed output: ${color.failure}%s${clear}", fail_output)
                        local actual_output = outdata
                        if not option.get("diagnosis") then
                            actual_output = outdata:sub(1, 64)
                            if #outdata > #actual_output then
                                actual_output = actual_output .. "..."
                            end
                        end
                        errors = errors .. ", actual output: ${color.failure}" .. actual_output
                    end
                    break
                end
            else
                if outdata:match("^" .. fail_output .. "$") then
                    passed = false
                    if not errors or #errors == 0 then
                        errors = string.format("matched failed output: ${color.failure}%s${clear}", fail_output)
                        local actual_output = outdata
                        if not option.get("diagnosis") then
                            actual_output = outdata:sub(1, 64)
                            if #outdata > #actual_output then
                                actual_output = actual_output .. "..."
                            end
                        end
                        errors = errors .. ", actual output: ${color.failure}" .. actual_output
                    end
                    break
                end
            end
        end
        if passed == nil then
            if #pass_outputs == 0 then
                passed = true
            else
                passed = false
                if not errors or #errors == 0 then
                    errors = string.format("not matched passed output: ${color.success}%s${clear}", table.concat(pass_outputs, ", "))
                    local actual_output = outdata
                    if not option.get("diagnosis") then
                        actual_output = outdata:sub(1, 64)
                        if #outdata > #actual_output then
                            actual_output = actual_output .. "..."
                        end
                    end
                    errors = errors .. ", actual output: ${color.failure}" .. actual_output
                end
            end
        end
        if errors and #errors > 0 then
            opt.errors = errors
            if option.get("verbose") or option.get("diagnosis") then
                cprint(errors)
            end
        end
        return passed
    end
    if errors and #errors > 0 then
        opt.errors = errors
        if option.get("verbose") or option.get("diagnosis") then
            cprint(errors)
        end
    end
    return false
end

-- test target
function _on_test_target(target, opt)

    -- build target with rules
    local passed
    local done = false
    for _, r in ipairs(target:orderules()) do
        local on_test = r:script("test")
        if on_test then
            passed = on_test(target, opt)
            done = true
        end
    end
    if done then
        return passed
    end

    -- do test
    return _do_test_target(target, opt)
end

-- recursively add target envs
function _add_target_pkgenvs(target, targets_added)
    if targets_added[target:name()] then
        return
    end
    targets_added[target:name()] = true
    os.addenvs(target:pkgenvs())
    for _, dep in ipairs(target:orderdeps()) do
        _add_target_pkgenvs(dep, targets_added)
    end
end

-- run the given test
function _run_test(target, test)

    -- enter the environments of the target packages
    local oldenvs = os.getenvs()
    _add_target_pkgenvs(target, {})

    -- the target scripts
    local scripts =
    {
        target:script("test_before")
    ,   function (target, opt)
            for _, r in ipairs(target:orderules()) do
                local before_test = r:script("test_before")
                if before_test then
                    before_test(target, opt)
                end
            end
        end
    ,   target:script("test", _on_test_target)
    ,   function (target, opt)
            for _, r in ipairs(target:orderules()) do
                local after_test = r:script("test_after")
                if after_test then
                    after_test(target, opt)
                end
            end
        end
    ,   target:script("test_after")
    }

    -- run the target scripts
    local passed
    local spent = 0
    for i = 1, 5 do
        local script = scripts[i]
        if script ~= nil then
            local cost = os.mclock()
            local ok = script(target, test)
            cost = os.mclock() - cost
            if i == 3 then
                passed = ok
                spent = cost
            end
        end
    end

    -- leave the environments of the target packages
    os.setenvs(oldenvs)
    return passed, spent
end

function _show_output(testinfo, kind)
    local output = testinfo[kind]
    if output then
        local target = testinfo.target
        local logfile = path.join(target:autogendir(), "tests", testinfo.name .. "." .. kind .. ".log")
        io.writefile(logfile, output)
        print("%s: %s", kind, logfile)
    end
end

-- process content for one test
function _progress_content(passed, opt, maxwidth, spent, testinfo)
    local status_color = passed and "${color.success}" or "${color.failure}"
    local progress_format = status_color .. theme.get("text.build.progress_format") .. ":${clear} "
    if option.get("verbose") then
        progress_format = progress_format .. "${dim}"
    end
    local progress = opt.progress:percent()
    local padding = maxwidth - #testinfo.name
    local process_content = format(progress_format .. "%s%s .................................... " .. status_color .. "%s${clear} ${bright}%0.3fs",
        progress, testinfo.name, (" "):rep(padding), passed and "passed" or "failed", spent / 1000)
    return process_content
end

-- run tests
function _run_tests(tests)
    local ordertests = {}  -- sort all tests by name
    local maxwidth = 0  -- calculate the max length of the test names
    for name, testinfo in table.orderpairs(tests) do
        table.insert(ordertests, testinfo)
        if #testinfo.name > maxwidth then
            maxwidth = #testinfo.name
        end
    end
    if #ordertests == 0 then
        print("nothing to test")
        return
    end

    -- do test
    local spent = os.mclock()
    print("running tests ...")
    local report = {passed = 0, total = #ordertests, tests = {}}
    local jobs = tonumber(option.get("jobs")) or os.default_njob()
    runjobs("run_tests", function (index, total, opt)
        local testinfo = ordertests[index]
        if testinfo then
            -- progress.show(opt.progress, "running.test %s", testinfo.name)

            local target = testinfo.target
            testinfo.target = nil
            local passed, spent = _run_test(target, testinfo)
            if passed then
                report.passed = report.passed + 1
            end
            local process_content = _progress_content(passed, opt, maxwidth, spent, testinfo)
            cprint(process_content)
            table.insert(report.tests, {
                target = target,
                name = testinfo.name,
                passed = passed,
                spent = spent,
                stdout = testinfo.stdout,
                stderr = testinfo.stderr,
                errors = testinfo.errors,
                process_content = process_content})

            -- stop it if be failed?
            if not passed then
                local stop = target:policy("test.stop_on_first_failure")
                if stop == nil then
                    stop = project.policy("test.stop_on_first_failure")
                end
                if stop then
                    raise(errors)
                end
            end
        end
    end, {total = #ordertests,
          comax = jobs,
          isolate = true})

    -- generate report
    spent = os.mclock() - spent
    local passed_rate = math.floor(report.passed * 100 / report.total)
    print("")
    print("report of tests:")
    for idx, testinfo in ipairs(report.tests) do
        cprint(testinfo.process_content)
        _show_output(testinfo, "stdout")
        _show_output(testinfo, "stderr")
        _show_output(testinfo, "errors")
    end
    cprint("${color.success}%d%%${clear} tests passed, ${color.failure}%d${clear} tests failed out of ${bright}%d${clear}, spent ${bright}%0.3fs",
        passed_rate, report.total - report.passed, report.total, spent / 1000)
    local return_zero = project.policy("test.return_zero_on_failure")
    if not return_zero and report.passed < report.total then
        raise()
    end
end

-- try to build the given target
function _try_build_target(targetname)
    local errors
    local passed = try {
        function ()
            build_action.build_targets(targetname)
            return true
        end,
        catch {
            function (errs)
                errors = tostring(errs)
            end
        }
    }
    return passed, errors
end


-- get tests, export this for the `project` plugin
function get_tests()
    local tests = {}
    local group_pattern = option.get("group")
    if group_pattern then
        group_pattern = "^" .. path.pattern(group_pattern) .. "$"
    end
    for _, target in ipairs(project.ordertargets()) do
        for _, name in ipairs(target:get("tests")) do
            local extra = target:extraconf("tests", name)
            local testname = target:name() .. "/" .. name
            local testinfo = {name = testname, target = target}
            if extra then
                table.join2(testinfo, extra)
                if extra.files then
                    local target_new = target:clone()
                    local scriptdir = target:scriptdir()
                    target_new:name_set(target:name() .. "_" .. name)
                    for _, file in ipairs(extra.files) do
                        file = path.absolute(file, scriptdir)
                        file = path.relative(file, os.projectdir())
                        target_new:add("files", file, {defines = extra.defines,
                                                       cflags = extra.cflags,
                                                       cxflags = extra.cxflags,
                                                       cxxflags = extra.cxxflags,
                                                       undefines = extra.undefines,
                                                       languages = extra.languages})
                        project.target_add(target_new)
                    end
                    for _, file in ipairs(extra.remove_files) do
                        file = path.absolute(file, scriptdir)
                        file = path.relative(file, os.projectdir())
                        target_new:remove("files", file)
                    end
                    if extra.kind then
                        target_new:set("kind", kind)
                    end
                    if extra.frameworks then
                        target_new:add("frameworks", extra.frameworks)
                    end
                    if extra.links then
                        target_new:add("links", extra.links)
                    end
                    if extra.syslinks then
                        target_new:add("syslinks", extra.syslinks)
                    end
                    if extra.packages then
                        target_new:add("packages", extra.packages)
                    end
                    testinfo.target = target_new
                end
            end
            if not testinfo.group then
                testinfo.group = target:get("group")
            end

            local group = testinfo.group
            if (not group_pattern) or (group_pattern and group and group:match(group_pattern)) then
                tests[testname] = testinfo
            end
        end
    end
    return tests
end

function main()
    -- do action for remote?
    if remote_build_action.enabled() then
        return remote_build_action()
    end

    -- lock the whole project
    project.lock()

    -- load config first
    task.run("config", {}, {disable_dump = true})

    -- get tests
    local tests = get_tests()
    local test_patterns = option.get("tests")
    if test_patterns then
        local tests_new = {}
        for _, pattern in ipairs(test_patterns) do
            pattern = "^" .. path.pattern(pattern) .. "$"
            for name, testinfo in pairs(tests) do
                if name:match(pattern) then
                    tests_new[name] = testinfo
                end
            end
        end
        tests = tests_new
    end

    -- enter project directory
    local oldir = os.cd(project.directory())

    -- build targets with the given tests with `build_should_pass` or `build_should_fail` tags firstly
    local targetnames = {}
    for _, testinfo in table.orderpairs(tests) do
        local targetname = testinfo.target:name()
        if testinfo.build_should_pass then
            local passed, errors = _try_build_target(targetname)
            testinfo.passed = passed
            testinfo.errors = errors
        elseif testinfo.build_should_fail then
            local built, _ = _try_build_target(targetname)
            testinfo.passed = not built
            if built then
                testinfo.errors = "Build succeeded when failure was expected"
            end
        else
            table.insert(targetnames, targetname)
        end
    end

    -- build the remaining tests
    if #targetnames > 0 then
        build_action.build_targets(targetnames)
    end

    -- run tests
    _run_tests(tests)

    -- leave project directory
    os.cd(oldir)

    -- unlock the whole project
    project.unlock()
end
