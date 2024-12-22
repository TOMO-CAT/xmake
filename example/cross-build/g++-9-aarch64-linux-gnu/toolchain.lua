toolchain("orin", function()
    set_toolset("cc", "clang")
    set_toolset("cxx", "clang++")
    set_toolset("cpp", "clang -E")
    set_toolset("as", "clang")
    set_toolset("sh", "clang++")
    set_toolset("ld", "clang")
    -- set_toolset("ar", "/usr/lib/llvm-14/bin/llvm-ar")
    -- set_toolset("strip", "/usr/lib/llvm-14/bin/llvm-strip")
    set_toolset("mm", "clang")
    set_toolset("mxx", "clang", "clang++")
    set_toolset("as", "clang")
    -- set_toolset("ranlib", "/usr/lib/llvm-14/bin/llvm-ranlib")
    -- set_toolset("cu", "/usr/local/cuda/bin/nvcc")
    -- set_toolset("culd", "/usr/local/cuda/bin/nvcc")
    -- set_toolset("cu-ccbin", "/usr/bin/aarch64-linux-gnu-gcc")

    add_syslinks("stdc++", "m")

    on_load(function(toolchain)
        os.setenv("C_INCLUDE_PATH", "")
        os.setenv("CPLUS_INCLUDE_PATH", "")
        os.setenv("LD_LIBRARY_PATH", "")
        os.setenv("LIBRARY_PATH", "")
        os.setenv("CC", "")
        os.setenv("CPP", "")
        os.setenv("CXX", "")
        os.setenv("LD", "")

        toolchain:add("cxflags", "-m64", "-fPIC",
                      "--sysroot=/usr/aarch64-linux-gnu",
                      "--target=aarch64-linux-gnu", "-march=armv8-a")
        toolchain:add("ldflags", "-m64", "--sysroot=/usr/aarch64-linux-gnu",
                      "--target=aarch64-linux-gnu", "-march=armv8-a")
        toolchain:add("shflags", "-m64", "--sysroot=/usr/aarch64-linux-gnu",
                      "-fPIC", "--target=aarch64-linux-gnu", "-march=armv8-a")
    end)
end)
