add_rules("mode.debug", "mode.release")
set_languages("c++latest")

target("mod", function()
    set_kind("static")
    add_files("src/*.cpp")
    add_files("src/*.mpp", {public = true})
end)

target("stdmodules", function()
    set_kind("binary")
    add_files("test/*.cpp")
    add_deps("mod")
end)
