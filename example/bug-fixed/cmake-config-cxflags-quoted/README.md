# CMake package with multiple configured `cxflags`

This example covers CMake package installation when multiple compiler flags
are passed through one `xmake config --cxflags=...` value.

Run:

```sh
./test.sh
```

The script configures the project with:

```text
--cxflags="-flto=full -fPIC"
```

The generated CMake configuration should contain:

```text
-DCMAKE_CXX_FLAGS=-flto=full -fPIC
```

The compiler should receive two separate arguments:

```text
clang++ -flto=full -fPIC ...
```

Before the fix, literal quotes were included in the CMake value and clang
received one combined argument:

```text
clang++ "-flto=full -fPIC" ...
unsupported argument 'full -fPIC' to option '-flto='
```
