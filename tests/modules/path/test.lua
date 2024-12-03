function test_splitenv_unix(t)
    t:are_equal(path.splitenv(""), {})
    t:are_equal(path.splitenv("a"), {'a'})
    t:are_equal(path.splitenv("a:b"), {'a', 'b'})
    t:are_equal(path.splitenv("::a::b:"), {'a', 'b'})
    t:are_equal(path.splitenv('a%tag:b'), {'a', 'b'})
    t:are_equal(path.splitenv('a%tag:b%tag'), {'a', 'b'})
    t:are_equal(path.splitenv('a%tag:b%%tag%%'), {'a', 'b'})
    t:are_equal(path.splitenv('a%tag:b:%tag:'), {'a', 'b'})
end

function test_extension(t)
    t:are_equal(path.extension("1.1/abc"), "")
    t:are_equal(path.extension("1.1\\abc"), "")
    t:are_equal(path.extension("foo.so"), ".so")
    t:are_equal(path.extension("/home/foo.so"), ".so")
    t:are_equal(path.extension("\\home\\foo.so"), ".so")
end

function test_directory(t)
    t:are_equal(path.directory(""), nil)
    t:are_equal(path.directory("."), nil)
    t:are_equal(path.directory("foo"), ".")

    t:are_equal(path.directory("/tmp"), "/")
    t:are_equal(path.directory("/tmp/"), "/")
    t:are_equal(path.directory("/tmp/xxx"), "/tmp")
    t:are_equal(path.directory("/tmp/xxx/"), "/tmp")
    t:are_equal(path.directory("/"), nil)
end

function test_absolute(t)
    t:are_equal(path.absolute("", ""), nil)
    t:are_equal(path.absolute(".", "."), ".")

    t:are_equal(path.absolute("", "/"), nil)
    t:are_equal(path.absolute("/", "/"), "/")
    t:are_equal(path.absolute(".", "/"), "/")
    t:are_equal(path.absolute("foo", "/tmp/"), "/tmp/foo")
    t:are_equal(path.absolute("foo", "/tmp"), "/tmp/foo")
end

function test_relative(t)
    t:are_equal(path.relative("", ""), nil)
    t:are_equal(path.relative(".", "."), ".")

    t:are_equal(path.relative("", "/"), nil)
    t:are_equal(path.relative("/", "/"), ".")
    t:are_equal(path.relative("/tmp/foo", "/tmp/"), "foo")
    t:are_equal(path.relative("/tmp/foo", "/tmp"), "foo")
end

function test_translate(t)
    t:are_equal(path.translate(""), nil)
    t:are_equal(path.translate("."), ".")
    t:are_equal(path.translate(".."), "..")

    t:are_equal(path.translate("/"), "/");
    t:are_equal(path.translate("////"), "/");
    t:are_equal(path.translate("/foo//////"), "/foo");
    t:are_equal(path.translate("/foo/../.."), "/foo/../..");
    t:are_equal(path.translate("/foo/../../"), "/foo/../..");
end

function test_normalize(t)
    t:are_equal(path.normalize("././."), ".")
    t:are_equal(path.normalize("../foo/.."), "..")
    t:are_equal(path.normalize("../foo/bar/../.."), "..")

    t:are_equal(path.normalize("/foo/././"), "/foo");
    t:are_equal(path.normalize("/./././"), "/");
    t:are_equal(path.normalize("/foo/bar/.//..//xyz"), "/foo/xyz");
    t:are_equal(path.normalize("/foo/../.."), "/");
    t:are_equal(path.normalize("/foo/bar../.."), "/foo");
    t:are_equal(path.normalize("../.."), "../..");
    t:are_equal(path.normalize("../foo/bar/.."), "../foo");
    t:are_equal(path.normalize("../foo/bar/../../.."), "../..");
end

function test_instance(t)
    t:are_equal(path("/tmp/a"):str(), "/tmp/a")
    t:are_equal(path("/tmp/a"):directory():str(), "/tmp")
    t:are_equal(path("/tmp/a", function(p)
        return "--key=" .. p
    end):str(), "--key=/tmp/a")
    t:are_equal(path("/tmp/a", function(p)
        return "--key=" .. p
    end):rawstr(), "/tmp/a")
    t:are_equal(path("/tmp/a", function(p)
        return "--key=" .. p
    end):clone():set("/tmp/b"):str(), "--key=/tmp/b")
end
