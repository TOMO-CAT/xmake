add_rules("mode.release", "mode.debug")
set_languages("c++20")

target("mod", function()
    set_kind("static")
    add_files("src/mod.mpp", {public = true})
    add_files("src/mod.cpp")
end)

target("cpp_with_moduledeps", function()
    set_kind("binary")
    add_deps("mod")
    add_files("src/main.cpp")
end)
