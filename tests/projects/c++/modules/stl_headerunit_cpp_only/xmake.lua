add_rules("mode.release", "mode.debug")
set_languages("c++20")

-- clang 编译 module 报错，暂时禁用
set_config("cc", "gcc")
set_config("cxx", "g++")

target("stl_headerunit_cpp_only", function()
    set_kind("binary")
    add_files("src/*.cpp")
    set_policy("build.c++.modules", true)
end)
