add_rules("mode.release")

toolchain("repro-clang", function()
    set_kind("cross")
    set_toolset("cc", "clang")
    set_toolset("cxx", "clang++")
    set_toolset("ld", "clang++")
    set_toolset("sh", "clang++")
    set_toolset("ar", "llvm-ar")
    set_toolset("ranlib", "llvm-ranlib")
end)

set_plat("cross")
set_arch("x86_64")
set_toolchains("repro-clang")

package("cmake-flags-repro", function()
    set_sourcedir(path.join(os.scriptdir(), "package"))
    set_policy("package.install_always", true)

    on_install(function(package)
        import("package.tools.cmake").install(package, {})
    end)
end)

add_requires("cmake-flags-repro", {system = false})

target("repro", function()
    set_kind("binary")
    add_files("src/main.cpp")
    add_packages("cmake-flags-repro")
end)
