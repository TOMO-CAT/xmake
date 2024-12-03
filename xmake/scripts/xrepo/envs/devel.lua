add_requires("cmake", "ninja")

add_requires("python 3.x")

if is_host("linux", "bsd", "macosx") then
    add_requires("pkg-config", "autoconf", "automake", "libtool")
end
