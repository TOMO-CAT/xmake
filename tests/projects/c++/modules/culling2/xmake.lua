add_rules("mode.release", "mode.debug")
set_languages("c++20")

target("culling", function()
    set_kind("static")
    add_files("src/*.mpp", {cull = false})
end)
