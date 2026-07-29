-- @see https://github.com/TOMO-CAT/xmake/issues/259

set_plat("linux")
set_arch("x86_64")
set_toolchains("repro-clangxx")
set_policy("build.across_targets_in_parallel", true)

toolchain("repro-clangxx", function()
    set_kind("standalone")
    set_toolset("cc", "clang")
    set_toolset("cxx", "clang++@" .. path.join(os.scriptdir(), "slow-clangxx.sh"))
    set_toolset("ld", "clang++@" .. path.join(os.scriptdir(), "slow-clangxx.sh"))
    set_toolset("sh", "clang++@" .. path.join(os.scriptdir(), "slow-clangxx.sh"))
end)

rule("repro.release", function()
    on_config(function(target)
        if is_mode("release") then
            target:add("cxxflags", "-Wall", "-Wextra", "-Werror",
                       "-Wno-unused-parameter", "-Wno-reorder-ctor",
                       "-Wno-deprecated-declarations", "-Wno-sign-compare",
                       "-Wno-defaulted-function-deleted",
                       "-Wno-absolute-value")
            target:add("cxxflags", "-Wno-missing-field-initializers",
                       "-Wno-enum-constexpr-conversion",
                       "-Wno-unknown-warning-option",
                       "-Wno-unused-but-set-variable",
                       "-Wno-c++11-narrowing-const-reference",
                       "-Wno-vla-cxx-extension", "-Wno-format",
                       "-Wno-packed-non-pod", "-include cstdint",
                       "-include optional", "-include array")
            target:add("cxflags", "-ffp-contract=fast", "-fno-math-errno",
                       "-fno-trapping-math", "-fno-rounding-math",
                       "-fno-signed-zeros")
            target:add("cxflags", "-fPIC")
            if not target:get("optimize") then
                target:set("optimize", "fastest")
            end
        end
    end)
end)

add_rules("repro.release")

for i = 1, 48 do
    target("repro" .. i, function()
        set_kind("object")
        add_files("main.cpp")
    end)
end
