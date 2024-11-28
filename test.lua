function new_job(name, childs)
    return {name = name, childs = childs, parents = {}, ancestors = {}}
end

local job_d = new_job("D", {})
local job_c = new_job("C", {})
local job_b = new_job("B", {job_c, job_d})
local job_a = new_job("A", {job_b, job_c, job_d})

function dfs(node, parent, parents, ancestors)
    table.insert(node.parents, parent)
    for _, p in ipairs(parents) do
        table.insert(node.ancestors, p)
    end
    for _, p in ipairs(ancestors) do
        table.insert(node.ancestors, p)
    end
    for _, child in ipairs(node.childs) do
        dfs(child, node, node.parents, node.ancestors)
    end
end
function main(root)
    for _, child in ipairs(root.childs) do
        dfs(child, root, root.parents, root.ancestors)
    end
end

function print_info(root)
    print('---------------------------------------------------')
    print("name: " .. root.name)
    print("childs")
    for _, c in ipairs(root.childs) do
        print(c.name)
    end
    print("parents")
    for _, c in ipairs(root.parents) do
        print(c.name)
    end
    print("ancestors")
    for _, c in ipairs(root.ancestors) do
        print(c.name)
    end
    for _, child in ipairs(root.childs) do
        print_info(child)
    end
end

main(job_a)
print_info(job_a)
