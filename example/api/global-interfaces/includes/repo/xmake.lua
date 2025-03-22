add_repositories("my-repo https://github.com/zxmake/cat-zxmake-repo.git")

includes("@my-repo/rules/release_mode.lua")

target("test", function()
    set_kind("binary")
    add_files("main.cc")
    add_rules("release_mode")
end)
