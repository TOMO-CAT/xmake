add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

target("unit_test", function()
    set_kind("binary")
    add_files("unit_test.cc")
    before_test(function()
        print("start prepare test data ...")
        os.sleep(10 * 1000) -- 10 seconds
        print("prepare test data done")
    end)
    add_tests("default", {run_timeout = 3 * 1000})  -- 3 seconds
end)
