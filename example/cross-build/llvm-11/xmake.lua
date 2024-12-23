add_requires("llvm 14.0.0", {alias = "llvm-10"})

target("test", function()
    set_kind("binary")
    add_files("src/*.cc")
    set_toolchains("llvm@llvm-10")
end)
