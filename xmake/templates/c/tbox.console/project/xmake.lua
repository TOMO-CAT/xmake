set_xmakever("2.3.6")
set_warnings("all", "error")
set_languages("c99", "cxx11")
add_cxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing")
add_mxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing")

add_rules("mode.release", "mode.debug")
add_requires("tbox", {debug = is_mode("debug")})

includes("src")
