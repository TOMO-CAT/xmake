package("git")
    set_kind("binary")
    set_homepage("https://git-scm.com/")
    set_description("A free and open source distributed version control system")

    on_load("windows", function (package)
        package:addenv("PATH", path.join("share", "MinGit", package:is_arch("x86_64", "x64") and "mingw64" or "mingw32", "bin"))
        package:addenv("PATH", path.join("share", "MinGit", "cmd"))
    end)

    on_install("macosx", "linux", "bsd", function (package)
        import("package.manager.install_package")("git")
    end)

    on_install("windows", function (package)
        os.cp("*", package:installdir("share/MinGit"))
    end)

    on_test(function (package)
        os.vrun("git --version")
    end)
