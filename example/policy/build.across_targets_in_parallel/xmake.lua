add_rules("mode.debug", "mode.release")
set_languages("c++17")
set_policy("build.across_targets_in_parallel", true)

-- binary target A -> object target B -> static target C
target("C", function()
    set_kind("static")
    add_files("src/C.cc")
end)

target("B", function()
    set_kind("object")
    add_files("src/B.cc")
    add_deps("C")
end)

target("A", function()
    set_kind("binary")
    add_files("src/A.cc")
    add_deps("B")
end)
