package("7z")

    set_kind("binary")
    set_homepage("https://www.7-zip.org/")
    set_description("A file archiver with a high compression ratio.")

    set_urls("https://github.com/xmake-mirror/7zip/archive/refs/tags/$(version).tar.gz",
                "https://github.com/xmake-mirror/7zip.git")
    add_versions("21.02", "b2a4c5bec8207508b26f94507f62f5a79c57ae9ab77dbf393f3b2fc8eef2e382")
    add_patches("21.02", path.join(os.scriptdir(), "patches", "21.02", "backport-21.03-fix-for-GCC-10.patch"), "f1d8fa0bbb25123b28e9b2842da07604238b77e51b918260a369f97c2f694c89")

    on_install("macosx", "linux", function (package)
        -- Clang has some indentation warnings that fails compilation using -Werror, remove it
        io.replace("CPP/7zip/7zip_gcc.mak", "CFLAGS_WARN_WALL = -Wall -Werror -Wextra", "CFLAGS_WARN_WALL = -Wall -Wextra", {plain = true})

        os.cd("CPP/7zip/Bundles/Alone2")
        os.vrun("make -j -f makefile.gcc")

        local bin = package:installdir("bin")
        os.cp("_o/7zz", bin)
        os.ln(bin .. "/7zz", bin .. "/7z")
    end)

    on_test(function (package)
        os.vrun("7z --help")
    end)
