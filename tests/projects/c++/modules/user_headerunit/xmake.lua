add_rules("mode.release", "mode.debug")
set_languages("c++20")

target("user_headerunit", function()
    set_kind("binary")
    add_headerfiles("src/*.hpp")
    add_files("src/*.cpp", "src/*.mpp")
end)
