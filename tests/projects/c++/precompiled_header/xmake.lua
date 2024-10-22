add_rules("mode.debug", "mode.release")

add_includedirs("src")

target("main", function()
    set_kind("binary")
    set_languages("cxx11")
    set_pcxxheader("src/header.h")
    add_files("src/*.cpp", "src/*.c", "*.cpp")
end)
