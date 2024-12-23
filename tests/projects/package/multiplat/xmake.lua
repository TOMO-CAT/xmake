add_rules("mode.debug", "mode.release")

add_requires("raylib")
add_requires("raylib~iphoneos", {plat = "iphoneos", arch = "x86_64"})

target("hello")
    add_packages("raylib")
    set_kind("binary")
    add_files("src/*.cpp")
    set_languages("c++11")

target("hello_iphoneos")
    set_plat("iphoneos")
    set_arch("x86_64")
    add_packages("raylib~iphoneos")
    set_kind("binary")
    add_files("src/*.cpp")
