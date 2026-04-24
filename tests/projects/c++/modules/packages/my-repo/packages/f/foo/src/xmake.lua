add_rules("mode.release", "mode.debug")
set_languages("c++20")

target("foo", function()
    set_kind("static")
    add_files("*.mpp", {public = true})
end)