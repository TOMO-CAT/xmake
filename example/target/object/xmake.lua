add_rules("mode.debug", "mode.release")
set_languages("c++17")

target("A", function()
    set_kind("object")
    add_files("src/1.cc")
end)
