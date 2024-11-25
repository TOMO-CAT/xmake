add_rules("mode.debug", "mode.release")

add_requires("go::github.com/sirupsen/logrus", {alias = "logrus"})
add_requires("go::golang.org/x/sys/internal/unsafeheader", {alias = "unsafeheader"})
add_requires("go::golang.org/x/sys/unix", {alias = "syshost"})


target("module")
    set_kind("static")
    add_files("src/test/*.go")
    add_packages("logrus", "syshost", "unsafeheader")

target("test")
    set_kind("binary")
    add_deps("module")
    add_files("src/*.go")
