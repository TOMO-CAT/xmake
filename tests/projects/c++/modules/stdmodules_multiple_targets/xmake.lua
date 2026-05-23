add_rules("mode.debug", "mode.release")
set_languages("c++latest")

target("mod", function()
    set_kind("static")
    add_files("src/*.cpp", "src/*.mpp")
end)

target("mod2", function()
    set_kind("static")
    add_files("src/*.cpp", "src/*.mpp")
end)
