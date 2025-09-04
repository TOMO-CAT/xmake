add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_includedirs(os.projectdir())

set_policy("build.across_targets_in_parallel", true)
set_policy("build.ccache", false)
set_policy("build.xcache", false)

-- add_cxxflags("-ftemplate-depth=3000")
-- add_cxxflags("-Wno-stack-exhausted")

target("slow_but_success", function()
    set_kind("binary")
    add_files("src/slow_but_success.cc")
end)

target("quick_but_fail", function()
    set_kind("binary")
    add_files("src/quick_but_fail.cc")
end)
