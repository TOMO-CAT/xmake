add_rules("mode.release", "mode.debug")
set_languages("c++20")

target("foo", function()
    set_kind("moduleonly")
    add_files("src/foo.mpp")
end)

target("bar", function()
    set_kind("moduleonly")
    add_files("src/bar.mpp")
end)

target("link_order_1", function()
    set_kind("binary")
    add_deps("foo", "bar")
    add_files("src/main.cpp")
end)

target("link_order_2", function()
    set_kind("binary")
    add_deps("bar", "foo")
    add_files("src/main.cpp")
end)
