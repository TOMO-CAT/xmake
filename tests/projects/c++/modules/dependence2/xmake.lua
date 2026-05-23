add_rules("mode.release", "mode.debug")
set_languages("c++20")

target("dependence2", function()
    set_kind("binary")
    add_files("src/*.cpp", "src/*.mpp")
end)
