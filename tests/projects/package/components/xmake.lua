add_rules("mode.debug", "mode.release")

add_requires("sfml")

target("graphics", function()
    set_kind("static")
    add_files("src/graphics.cpp")
    add_packages("sfml", {components = "graphics", public = true})
end)

target("network", function()
    set_kind("static")
    add_files("src/network.cpp")
    add_packages("sfml", {components = "network", public = true})
end)

target("test", function()
    set_kind("binary")
    add_files("src/main.cpp")
    add_deps("graphics", "network")
end)

package("xtrans", function()
    set_kind("library", {headeronly = true})
    set_homepage("https://www.x.org/")
    set_description("X.Org: X Network Transport layer shared code")

    set_urls("https://www.x.org/archive/individual/lib/xtrans-$(version).tar.gz")
    add_versions("1.4.0", "48ed850ce772fef1b44ca23639b0a57e38884045ed2cbb18ab137ef33ec713f9")

    if is_plat("linux") then
        add_extsources("apt::xtrans-dev", "pacman::xtrans")
    end

    if is_plat("macosx", "linux", "bsd") then
        add_deps("pkg-config", "util-macros", "xorgproto")
    end

    on_install("macosx", "linux", "bsd", function (package)
        local configs = {"--sysconfdir=" .. package:installdir("etc"),
                         "--localstatedir=" .. package:installdir("var"),
                         "--disable-dependency-tracking",
                         "--disable-silent-rules",
                         "--enable-docs=no"}
        -- fedora systems do not provide sys/stropts.h
        io.replace("Xtranslcl.c", "# include <sys/stropts.h>", "# include <sys/ioctl.h>")
        import("package.tools.autoconf").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_ctypes("Xtransaddr", {includes = "X11/Xtrans/Xtrans.h"}))
    end)
end)

package("sfml", function()
    set_homepage("https://www.sfml-dev.org")
    set_description("Simple and Fast Multimedia Library")

    if is_plat("linux") then
        set_urls("https://www.sfml-dev.org/files/SFML-$(version)-sources.zip")
        add_urls("https://github.com/SFML/SFML/releases/download/$(version)/SFML-$(version)-sources.zip")
        add_versions("2.5.1", "bf1e0643acb92369b24572b703473af60bac82caf5af61e77c063b779471bb7f")
    elseif is_plat("macosx") then
        if is_arch("x64", "x86_64") then
            set_urls("https://www.sfml-dev.org/files/SFML-$(version)-macOS-clang.tar.gz")
            add_versions("2.5.1", "6af0f14fbd41dc038a00d7709f26fb66bb7ccdfe6187657ef0ef8cba578dcf14")

            add_configs("debug", {builtin = true, description = "Enable debug symbols.", default = false, type = "boolean", readonly = true})
            add_configs("shared", {description = "Build shared library.", default = true, type = "boolean", readonly = true})
        end
    end

    if is_plat("linux") then
        add_syslinks("pthread")
    end

    add_configs("graphics",   {description = "Use the graphics module", default = true, type = "boolean"})
    add_configs("window",     {description = "Use the window module", default = true, type = "boolean"})
    add_configs("audio",      {description = "Use the audio module", default = true, type = "boolean"})
    add_configs("network",    {description = "Use the network module", default = true, type = "boolean"})

    if is_plat("macosx") then
        add_extsources("brew::sfml/sfml-all")
    end

    on_component("graphics", function (package, component)
        local e = package:config("shared") and "" or "-s"
        if package:debug() then
            e = e .. "-d"
        end
        component:add("links", "sfml-graphics" .. e)
        component:add("deps", "window", "system")
        component:add("extsources", "brew::sfml/sfml-graphics")
    end)

    on_component("window", function (package, component)
        local e = package:config("shared") and "" or "-s"
        if package:debug() then
            e = e .. "-d"
        end
        component:add("links", "sfml-window" .. e)
        component:add("deps", "system")
        component:add("extsources", "brew::sfml/sfml-window")
    end)

    on_component("audio", function (package, component)
        local e = package:config("shared") and "" or "-s"
        if package:debug() then
            e = e .. "-d"
        end
        component:add("links", "sfml-audio" .. e)
        component:add("deps", "system")
        component:add("extsources", "brew::sfml/sfml-audio")
    end)

    on_component("network", function (package, component)
        local e = package:config("shared") and "" or "-s"
        if package:debug() then
            e = e .. "-d"
        end
        component:add("links", "sfml-network" .. e)
        component:add("deps", "system")
        component:add("extsources", "brew::sfml/sfml-network")
        component:add("extsources", "apt::sfml-network")
    end)

    on_component("system", function (package, component)
        local e = package:config("shared") and "" or "-s"
        if package:debug() then
            e = e .. "-d"
        end
        component:add("links", "sfml-system" .. e)
        component:add("extsources", "brew::sfml/sfml-system")
    end)

    on_component("main", function (package, component)
    end)

    on_load("linux", "macosx", "mingw", function (package)
        if package:is_plat("linux") then
            package:add("deps", "cmake")
        end

        if not package:config("shared") then
            package:add("defines", "SFML_STATIC")
        end

        if package:is_plat("linux") then
            if package:config("window") or package:config("graphics") then
                package:add("deps", "libx11", "libxext", "libxrandr", "libxrender", "freetype", "eudev")
                package:add("deps", "opengl", "glx", {optional = true})
            end
            if package:config("audio") then
                package:add("deps", "libogg", "libflac", "libvorbis", "openal-soft")
            end
        end
        package:add("components", "system")
        for _, component in ipairs({"graphics", "window", "audio", "network"}) do
            if package:config(component) then
                package:add("components", component)
            end
        end
    end)

    on_install("linux", function (package)
        local configs = {"-DSFML_BUILD_DOC=OFF", "-DSFML_BUILD_EXAMPLES=OFF"}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        if package:config("shared") then
            table.insert(configs, "-DBUILD_SHARED_LIBS=ON")
        else
            table.insert(configs, "-DBUILD_SHARED_LIBS=OFF")
        end
        local packagedeps
        if package:is_plat("linux") and package:config("shared") then
            io.replace("src/SFML/Graphics/CMakeLists.txt", "target_link_libraries(sfml-graphics PRIVATE X11)",
                "target_link_libraries(sfml-graphics PRIVATE X11 Xext Xrender)", {plain = true})
            packagedeps = {"libxext", "libxrender"}
        end
        table.insert(configs, "-DSFML_BUILD_AUDIO=" .. (package:config("audio") and "ON" or "OFF"))
        table.insert(configs, "-DSFML_BUILD_GRAPHICS=" .. (package:config("graphics") and "ON" or "OFF"))
        table.insert(configs, "-DSFML_BUILD_WINDOW=" .. (package:config("window") and "ON" or "OFF"))
        table.insert(configs, "-DSFML_BUILD_NETWORK=" .. (package:config("network") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs, {packagedeps = packagedeps})
    end)

    on_install("macosx", "mingw", function (package)
        os.cp("lib", package:installdir())
        os.cp("include", package:installdir())
        if package:is_plat("mingw") then
            os.cp("bin/*", package:installdir("lib"), {rootdir = "bin"})
        end
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test(int args, char** argv) {
                sf::Clock c;
                c.restart();
            }
        ]]}, {includes = "SFML/System.hpp"}))
        if package:config("graphics") then
            assert(package:check_cxxsnippets({test = [[
                void test(int args, char** argv) {
                    sf::Text text;
                    text.setString("Hello world");
                }
            ]]}, {includes = "SFML/Graphics.hpp"}))
        end
        if package:config("window") or package:config("graphics") then
            assert(package:check_cxxsnippets({test = [[
                void test(int args, char** argv) {
                    sf::Window window(sf::VideoMode(1280, 720), "Title");

                    sf::Event event;
                    window.pollEvent(event);
                }
            ]]}, {includes = "SFML/Window.hpp"}))
        end
        if package:config("audio") then
            assert(package:check_cxxsnippets({test = [[
                void test(int args, char** argv) {
                    sf::Music music;
                    music.openFromFile("music.ogg");
                    music.play();
                }
            ]]}, {includes = "SFML/Audio.hpp"}))
        end
        if package:config("network") then
            assert(package:check_cxxsnippets({test = [[
                void test(int args, char** argv) {
                    sf::UdpSocket socket;
                    socket.bind(54000);

                    char data[100];
                    std::size_t received;
                    sf::IpAddress sender;
                    unsigned short port;
                    socket.receive(data, 100, received, sender, port);
                }
            ]]}, {includes = "SFML/Network.hpp"}))
        end
    end)
end)

package("libsndio", function()
    set_homepage("https://sndio.org")
    set_description("Sndio is a small audio and MIDI framework part of the OpenBSD project and ported to FreeBSD, Linux and NetBSD")

    set_urls("https://sndio.org/sndio-$(version).tar.gz")

    add_versions("1.9.0", "f30826fc9c07e369d3924d5fcedf6a0a53c0df4ae1f5ab50fe9cf280540f699a")

    if is_plat("linux") then
        add_deps("alsa-lib")
    end


    on_install("linux", "bsd", "macosx", function (package)
        import("package.tools.autoconf")
        local configs = {}
        local buildenvs = autoconf.buildenvs(package, {packagedeps = "alsa-lib"})
        if not package:config("shared") then
            io.replace("libsndio/Makefile.in",
                "${CC} ${LDFLAGS} ${SO_CFLAGS} ${SO_LDFLAGS} -o ${SO} ${OBJS} ${LDADD}",
                "${AR} ${ARFLAGS} libsndio.a ${OBJS}", {plain = true})
            io.replace("libsndio/Makefile.in", "cp -R ${SO} ${SO_LINK} ${DESTDIR}${LIB_DIR}",
                "cp libsndio.a ${DESTDIR}${LIB_DIR}", {plain = true})
        end
        autoconf.configure(package, configs, {envs = buildenvs})
        os.vrunv("make", {}, {envs = buildenvs})
        os.vrunv("make", {"install"}, {envs = buildenvs})
    end)

    -- on_test(function (package)
    --     assert(package:has_cfuncs("sio_open", {includes = "sndio.h"}))
    -- end)
end)

package("alsa-lib", function()
    set_homepage("https://alsa-project.org/wiki/Main_Page")
    set_description("The Advanced Linux Sound Architecture (ALSA) provides audio and MIDI functionality to the Linux operating system.")
    set_license("LGPL-2.1")

    add_urls("http://www.alsa-project.org/files/pub/lib/alsa-lib-$(version).tar.bz2", {alias = "home"})
    add_urls("https://github.com/alsa-project/alsa-lib/archive/refs/tags/v$(version).tar.gz", {alias = "github"})
    add_versions("home:1.2.10", "c86a45a846331b1b0aa6e6be100be2a7aef92efd405cf6bac7eef8174baa920e")
    add_versions("github:1.2.10", "f55749847fd98274501f4691a2d847e89280c07d40a43cdac43d6443f69fc939")

    add_configs("versioned", {description = "pass flag --without-versioned", default = true, type = "boolean"})

    if is_plat("linux") then
        -- https://stackoverflow.com/questions/9145264/undefined-reference-to-log
        add_syslinks("pthread", "m")
        add_extsources("pacman::alsa-lib", "apt::libasound2-dev")
    end

    if not is_plat("windows") then
        add_deps("autoconf", "automake", "libtool", "m4")
    end

    on_install("linux", function (package)
        local configs = {}
        table.insert(configs, "--enable-shared=" .. (package:config("shared") and "yes" or "no"))
        table.insert(configs, "--enable-static=" .. (package:config("shared") and "no" or "yes"))
        if package:is_debug() then
            table.insert(configs, "--enable-debug")
        end
        if package:config("versioned")then
            table.insert(configs, "--without-versioned")
        end
        import("package.tools.autoconf").install(package, configs)
    end)

    -- on_test(function (package)
    --     assert(package:has_cfuncs("snd_pcm_open", {includes = "alsa/asoundlib.h"}))
    -- end)
end)
