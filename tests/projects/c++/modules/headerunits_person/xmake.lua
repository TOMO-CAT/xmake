add_rules("mode.release", "mode.debug")
set_languages("c++20")

-- clang 编译 module 报错，暂时禁用
set_config("cc", "gcc")
set_config("cxx", "g++")

target("headerunits_person", function()
    set_kind("binary")
    add_files("src/*.cpp", "src/*.mpp")
end)
