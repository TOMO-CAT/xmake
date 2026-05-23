set_languages("c++20")

target("A", function()
    set_kind("moduleonly")
    add_files("src/modA.mpp")
end)

target("B", function()
    add_deps("A")
    set_kind("static")
    add_files("src/modB.mpp", { public = true })
    add_files("src/modB.cpp")
end)

target("test", function()
    set_kind("binary")
    add_deps("B")
    add_files("src/main.cpp")
end)
