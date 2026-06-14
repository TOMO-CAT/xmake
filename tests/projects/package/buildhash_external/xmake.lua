add_repositories("myrepo ./repo")
add_requires("foo", {system = false, build = true})

target("test", function()
    set_kind("binary")
    add_files("src/main.cpp")
    add_packages("foo")
end)
