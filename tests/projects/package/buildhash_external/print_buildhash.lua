import("core.base.task")
import("private.action.require.impl.package", {alias = "require_package"})
import("core.project.project")

-- print the buildhash of package(foo) for the current `xmake config`, prefixed with a stable marker
-- so the test can extract it reliably regardless of any other output on stdout.
task.run("config", {}, {disable_dump = true})
local requires, requires_extra = project.requires_str()
local packages = require_package.load_packages(requires, {requires_extra = requires_extra})
for _, pkg in ipairs(packages) do
    if pkg:name() == "foo" then
        print("BUILDHASH=" .. pkg:buildhash())
        return
    end
end
raise("package(foo) not found!")
