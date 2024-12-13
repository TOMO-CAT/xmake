add_rules("mode.debug", "mode.release", "mode.coverage")

set_languages("c++17")

add_rules("unknown-project-rules")

target("demo", function()
    set_kind("object")
    add_files("src/1.cc")
end)
