-- 单测 target
-- @see https://github.com/TOMO-CAT/xmake/issues/210
rule("unittest", function()
    on_config(function(target)
        if target:kind() ~= "test" then
            return
        end

        target:set("kind", "binary")
        target:set("default", false)

        -- 目前 add_testfiles 会自动进入沙盒模式, 禁止用户再自定义 rundir
        if target:get("rundir") and target:get("testfiles") then
            raise("rundir and testfiles cannot be used together in target [%s]",
                  target:name())
        end

        -- 没有调用 add_testfiles 时默认运行环境在 os.projectdir(), 相对路径引用单测文件
        if not target:get("rundir") then
            if target:get("testfiles") then
                local ori_rundir = path.absolute(target:rundir())
                local sandbox_rundir = path.join(ori_rundir,
                                                 target:name() .. ".rundir")
                os.tryrm(sandbox_rundir)
                os.mkdir(sandbox_rundir)
                target:set("rundir", sandbox_rundir)

                -- 将这个 target 标记成 sandbox 模式
                target:set("values", "sandbox", true)
            else
                target:set("rundir", os.projectdir())
            end
        end

        -- 如果只有一个 test (绝大多数场景), 不需要显式写 add_tests, 默认添加一个 default tests
        if not target:get("tests") then
            target:add("tests", "default")
        end
    end)

    before_run("prepare_test_data")
    before_test("prepare_test_data")
end)
