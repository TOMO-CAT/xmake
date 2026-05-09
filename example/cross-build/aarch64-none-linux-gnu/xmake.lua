add_rules("mode.debug", "mode.release")

set_languages("c++17")

target("aarch64-none-linux-gnu-cross", function()
    set_kind("binary")
    add_files("src/*.cc")
end)
