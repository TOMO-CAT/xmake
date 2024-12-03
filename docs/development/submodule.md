# submodule

## 常见操作

```bash
# 撤销对子模块的修改
git submodule foreach --recursive git checkout .
```

## 删除单个 submodule

以删除 Lua 子模块为例：

```bash
# 先使原先子模块不被版本控制
git rm --cached core/src/lua/lua

# 删除 submodule 的跟踪
git submodule deinit -f core/src/lua/lua

# 删除子模块目录
rm -rf core/src/lua/lua

# 删除 .gitmodules 文件中的子模块配置

# 修改 .git/config 内容, 把 submodule 对应配置删除

# 提交修改
git add .gitmodules

# 删除子模块的 git 目录
rm -rf .git/modules/core/src/lua

# 推送到远程仓库
git push
```
