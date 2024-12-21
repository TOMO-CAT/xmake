# Builtin Moduels

## 简介

定义在 `xmake/core/base/interpreter.lua` 中，目前包括：

```lua
-- get builtin modules
function interpreter.builtin_modules()
    local builtin_modules = interpreter._BUILTIN_MODULES
    if builtin_modules == nil then
        builtin_modules = {}
        local builtin_module_files = os.match(path.join(os.programdir(), "core/sandbox/modules/interpreter/*.lua"))
        if builtin_module_files then
            for _, builtin_module_file in ipairs(builtin_module_files) do
                local module_name = path.basename(builtin_module_file)
                assert(module_name)

                local script, errors = loadfile(builtin_module_file)
                if script then
                    local ok, results = utils.trycall(script)
                    if not ok then
                        os.raise(results)
                    end
                    builtin_modules[module_name] = results
                else
                    os.raise(errors)
                end
            end
        end
        interpreter._BUILTIN_MODULES = builtin_modules
    end
    return builtin_modules
end
```

内容：

```lua
$ utils.dump(interpreter._BUILTIN_MODULES)
{ 
  print = function ...l/share/xmake/core/sandbox/modules/interpreter/print.lua:27,
  pairs = function /usr/local/share/xmake/core/sandbox/modules/pairs.lua:25,
  os = { 
    term = function /usr/local/share/xmake/core/base/os.lua:1186,
    arch = function /usr/local/share/xmake/core/base/os.lua:1023,
    default_njob = function /usr/local/share/xmake/core/base/os.lua:1410,
    curdir = function [C],
    host = function /usr/local/share/xmake/core/base/os.lua:1018,
    tmpdir = function /usr/local/share/xmake/core/base/os.lua:628,
    projectdir = function /usr/local/share/xmake/core/base/os.lua:1450,
    programdir = function /usr/local/share/xmake/core/base/os.lua:1435,
    programfile = function /usr/local/share/xmake/core/base/os.lua:1440,
    getenv = function [C],
    filedirs = function ...ocal/share/xmake/core/sandbox/modules/interpreter/os.lua:64,
    mclock = function [C],
    isdir = function [C],
    time = function [C],
    dirs = function ...ocal/share/xmake/core/sandbox/modules/interpreter/os.lua:59,
    subarch = function /usr/local/share/xmake/core/base/os.lua:1033,
    subhost = function /usr/local/share/xmake/core/base/os.lua:1028,
    cpuinfo = function /usr/local/share/xmake/core/base/os.lua:1400,
    filesize = function [C],
    mtime = function [C],
    date = function [C],
    projectfile = function /usr/local/share/xmake/core/base/os.lua:1455,
    scriptdir = function ...ocal/share/xmake/core/sandbox/modules/interpreter/os.lua:69,
    exists = function [C],
    isfile = function [C],
    files = function ...ocal/share/xmake/core/sandbox/modules/interpreter/os.lua:54 
  },
  macos = { 
    version = function /usr/local/share/xmake/core/base/macos.lua:29 
  },
  format = function [C],
  tonumber = function [C],
  getenv = function [C],
  table = { 
    wrap_lock = function /usr/local/share/xmake/core/base/table.lua:366,
    append = function /usr/local/share/xmake/core/base/table.lua:149,
    is_dictionary = function /usr/local/share/xmake/core/base/table.lua:270,
    new = function /usr/local/share/xmake/core/base/table.lua:42,
    to_array = function /usr/local/share/xmake/core/base/table.lua:322,
    map = function /usr/local/share/xmake/core/base/table.lua:525,
    sort = function [C],
    orderpairs = function /usr/local/share/xmake/core/base/table.lua:500,
    find = function /usr/local/share/xmake/core/base/table.lua:576,
    shallow_join2 = function /usr/local/share/xmake/core/base/table.lua:134,
    is_array = function /usr/local/share/xmake/core/base/table.lua:265,
    orderkeys = function /usr/local/share/xmake/core/base/table.lua:480,
    join2 = function /usr/local/share/xmake/core/base/table.lua:109,
    find_first_if = function /usr/local/share/xmake/core/base/table.lua:627,
    move = function [C],
    remove = function [C],
    insert = function [C],
    pack = function /usr/local/share/xmake/core/base/table.lua:437,
    wrap = function /usr/local/share/xmake/core/base/table.lua:352,
    shallow_join = function /usr/local/share/xmake/core/base/table.lua:124,
    imap = function /usr/local/share/xmake/core/base/table.lua:534,
    slice = function /usr/local/share/xmake/core/base/table.lua:256,
    copy2 = function /usr/local/share/xmake/core/base/table.lua:183,
    swap = function /usr/local/share/xmake/core/base/table.lua:142,
    concat = function [C],
    unique = function /usr/local/share/xmake/core/base/table.lua:382,
    contains = function /usr/local/share/xmake/core/base/table.lua:276,
    find_if = function /usr/local/share/xmake/core/base/table.lua:597,
    copy = function /usr/local/share/xmake/core/base/table.lua:173,
    find_first = function /usr/local/share/xmake/core/base/table.lua:618,
    empty = function /usr/local/share/xmake/core/base/table.lua:571,
    remove_if = function /usr/local/share/xmake/core/base/table.lua:553,
    clear = function /usr/local/share/xmake/core/base/table.lua:29,
    unpack = function [C],
    getn = function /usr/local/share/xmake/core/base/table.lua:51,
    values = function /usr/local/share/xmake/core/base/table.lua:514,
    maxn = function /usr/local/share/xmake/core/base/table.lua:58,
    reverse = function /usr/local/share/xmake/core/base/table.lua:543,
    reverse_unique = function /usr/local/share/xmake/core/base/table.lua:408,
    clone = function /usr/local/share/xmake/core/base/table.lua:160,
    unwrap = function /usr/local/share/xmake/core/base/table.lua:342,
    inherit2 = function /usr/local/share/xmake/core/base/table.lua:231,
    inherit = function /usr/local/share/xmake/core/base/table.lua:205,
    keys = function /usr/local/share/xmake/core/base/table.lua:446,
    join = function /usr/local/share/xmake/core/base/table.lua:93,
    wrap_unlock = function /usr/local/share/xmake/core/base/table.lua:374 
  },
  hash = { 
    xxhash64 = function /usr/local/share/xmake/core/sandbox/modules/hash.lua:70,
    xxhash128 = function /usr/local/share/xmake/core/sandbox/modules/hash.lua:79,
    uuid4 = function /usr/local/share/xmake/core/sandbox/modules/hash.lua:34,
    uuid = function /usr/local/share/xmake/core/sandbox/modules/hash.lua:29,
    md5 = function /usr/local/share/xmake/core/sandbox/modules/hash.lua:61,
    sha256 = function /usr/local/share/xmake/core/sandbox/modules/hash.lua:52,
    sha1 = function /usr/local/share/xmake/core/sandbox/modules/hash.lua:43 
  },
  unpack = function [C],
  is_subhost = function /usr/local/share/xmake/core/base/os.lua:1053,
  xmake = { 
    luajit = function /usr/local/share/xmake/core/base/xmake.lua:66,
    gitsource = function /usr/local/share/xmake/core/base/xmake.lua:51,
    programdir = function /usr/local/share/xmake/core/base/xmake.lua:56,
    programfile = function /usr/local/share/xmake/core/base/xmake.lua:61,
    version = function /usr/local/share/xmake/core/base/xmake.lua:33 
  },
  tostring = function [C],
  ipairs = function /usr/local/share/xmake/core/sandbox/modules/ipairs.lua:25,
  type = function [C],
  math = { 
    sin = function [C],
    deg = function [C],
    min = function [C],
    random = function [C],
    log = function [C],
    max = function [C],
    cos = function [C],
    tanh = function [C],
    isint = function /usr/local/share/xmake/core/base/math.lua:33,
    log10 = function [C],
    pow = function [C],
    rad = function [C],
    sqrt = function [C],
    pi = 3.1415926535898,
    fmod = function [C],
    ceil = function [C],
    floor = function [C],
    huge = inf,
    frexp = function [C],
    mininteger = -9223372036854775808,
    tointeger = function [C],
    asin = function [C],
    exp = function [C],
    cosh = function [C],
    isnan = function /usr/local/share/xmake/core/base/math.lua:57,
    isinf = function /usr/local/share/xmake/core/base/math.lua:42,
    inf = inf,
    randomseed = function [C],
    e = 2.718281828459,
    nan = -nan,
    maxinteger = 9223372036854775807,
    ult = function [C],
    abs = function [C],
    ldexp = function [C],
    sinh = function [C],
    atan2 = function [C],
    type = function [C],
    tan = function [C],
    modf = function [C],
    atan = function [C],
    acos = function [C] 
  },
  is_host = function /usr/local/share/xmake/core/base/os.lua:1043,
  string = { 
    startswith = function [C],
    ipattern = function /usr/local/share/xmake/core/base/string.lua:180,
    lower = function [C],
    format = function [C],
    sub = function [C],
    rep = function [C],
    replace = function /usr/local/share/xmake/core/base/string.lua:145,
    vformat = function /usr/local/share/xmake/core/sandbox/modules/string.lua:37,
    find = function [C],
    deserialize = function /usr/local/share/xmake/core/base/string.lua:258,
    serialize = function /usr/local/share/xmake/core/base/string.lua:248,
    len = function [C],
    wcwidth = function /usr/local/share/xmake/core/base/string.lua:263,
    decode = function /usr/local/share/xmake/core/base/string.lua:140,
    wcswidth = function /usr/local/share/xmake/core/base/string.lua:373,
    pack = function [C],
    upper = function [C],
    dump = function /usr/local/share/xmake/core/base/string.lua:236,
    endswith = function [C],
    convert = function [C],
    match = function [C],
    packsize = function [C],
    tryformat = function /usr/local/share/xmake/core/base/string.lua:160,
    unpack = function [C],
    lastof = function /usr/local/share/xmake/core/base/string.lua:36,
    gsub = function [C],
    rtrim = function /usr/local/share/xmake/core/base/string.lua:130,
    reverse = function [C],
    encode = function /usr/local/share/xmake/core/base/string.lua:135,
    byte = function [C],
    trim = function /usr/local/share/xmake/core/base/string.lua:120,
    split = function /usr/local/share/xmake/core/base/string.lua:82,
    ltrim = function /usr/local/share/xmake/core/base/string.lua:125,
    gmatch = function [C],
    levenshtein = function /usr/local/share/xmake/core/base/string.lua:386,
    char = function [C] 
  },
  linuxos = { 
    kernelver = function /usr/local/share/xmake/core/base/linuxos.lua:186,
    name = function /usr/local/share/xmake/core/base/linuxos.lua:80,
    version = function /usr/local/share/xmake/core/base/linuxos.lua:135 
  },
  printf = function .../share/xmake/core/sandbox/modules/interpreter/printf.lua:22,
  path = { 
    _translate = function [C],
    normalize = function /usr/local/share/xmake/core/base/path.lua:191,
    basename = function /usr/local/share/xmake/core/base/path.lua:249,
    translate = function /usr/local/share/xmake/core/base/path.lua:177,
    _relative = function [C],
    envsep = function /usr/local/share/xmake/core/base/path.lua:301,
    directory = function [C],
    instance_of = function /usr/local/share/xmake/core/base/path.lua:415,
    sep = function /usr/local/share/xmake/core/base/path.lua:291,
    filename = function /usr/local/share/xmake/core/base/path.lua:232,
    relative = function /usr/local/share/xmake/core/base/path.lua:224,
    new = function /usr/local/share/xmake/core/base/path.lua:410,
    islastsep = function /usr/local/share/xmake/core/base/path.lua:371,
    splitenv = function /usr/local/share/xmake/core/base/path.lua:312,
    cygwin = function /usr/local/share/xmake/core/base/path.lua:166,
    is_absolute = function [C],
    joinenv = function /usr/local/share/xmake/core/base/path.lua:349,
    cygwin_path = function /usr/local/share/xmake/core/base/path.lua:399,
    pattern = function /usr/local/share/xmake/core/base/path.lua:378,
    _absolute = function [C],
    extension = function /usr/local/share/xmake/core/base/path.lua:261,
    split = function /usr/local/share/xmake/core/base/path.lua:285,
    _SEP = "/",
    unix = function /usr/local/share/xmake/core/base/path.lua:161,
    join = function /usr/local/share/xmake/core/base/path.lua:279,
    absolute = function /usr/local/share/xmake/core/base/path.lua:216 
  } 
}
```
