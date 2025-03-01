add_rules("mode.debug", "mode.release")

add_requires("wxwidgets")

target("${TARGETNAME}", function()
    set_kind("binary")
    add_files("src/*.cpp")
    set_languages("c++14")
    add_packages("wxwidgets")
end)
