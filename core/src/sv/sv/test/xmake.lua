set_default(false)
set_languages("c99")
set_kind("binary")
add_deps("sv")
add_includedirs("../include")

target("version_test", function()
    add_files("version.c")
end)

target("comp_test", function()
    add_files("comp.c")
end)

target("range_test", function()
    add_files("range.c")
end)

target("match_test", function()
    add_files("match.c")
end)

target("semvers_test", function()
    add_files("semvers.c")
end)

target("utils_test", function()
    add_files("utils.c")
end)

target("usage_test", function()
    add_files("usage.c")
end)

task("check", function()
    on_run(function ()
        import("core.project.task")
        task.run("run", {target = "version_test"})
        task.run("run", {target = "comp_test"})
        task.run("run", {target = "range_test"})
        task.run("run", {target = "match_test"})
        task.run("run", {target = "semvers_test"})
        task.run("run", {target = "utils_test"})
        task.run("run", {target = "usage_test"})
    end)
    set_menu {
        usage = "xmake check"
    ,   description = "Run tests !"
    ,   options = {}
    }
end)
