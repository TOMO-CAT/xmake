# Todo

## 自动补全功能

按这个 issue [#5259](https://github.com/xmake-io/xmake/issues/5259) 的描述和[A Tour of xmake](https://zhuanlan.zhihu.com/p/548734585)，xmake 有自动补全和虚拟环境的功能，但是源码安装的目前没有自动补全，后面再研究。

## build.fence policy 导致 cpu 利用率不高

当前假设 target A 依赖 target B，而 target B 是一个 proto / codegen target 导致 target A 必须依赖 target B 产出的 .h 文件才能编译。那么我们往往要给 target B 设置 `build.fence` policy 为 true，当前这个策略会导致 target A 的 `before_build` job 需要等待 target B 的 `link` job 运行完才能开始运行。

尽管这没什么必要，但是当前的 batchjobs 不太好修改。

一种考虑是对于 target B 增加一个 `after_build_file` job，然后让 target A 的 `before_build` job 依赖 target B 的 `after_build_file` job。

## ✅ 优化一下 jobpool 的 __tostring() 方法

当前打印出来的依赖树阅读起来还是比较费劲，争取打印成 json 格式。

## 优化一下 utils.dump 打印 table 的逻辑

可以设置打印的 table 深度，免得打印出来的递归 table 太大。

## ✅ test.sh 增加运行全部 example 的命令

增强代码稳健性。

直接运行：

```bash
bash scripts/example.sh
```

## ✅ xmake f 支持增量添加配置

相关 Issue：

> <https://github.com/xmake-io/xmake/issues/5767>
> <https://github.com/xmake-io/xmake/issues/5041>

一种简单的方案是将这个第三方 plugin 内置到 xmake 里。

> <https://github.com/SirLynix/xmake-plugins>

通过 `config-update` 插件来支持增量配置：

```bash
xmake cu -m coverage
xmake cu --cxxflags="-DTEST -O3 -Wall"
```

## ✅ local package 无法复用 ccache 缓存

假设我们通过 `set_sourcedir` 的方式构造本地 package，但是编译完后发现 cache 命中率为零。

```lua
package("my-package", function()
    set_sourcedir("../my-package")
    on_install(function(package)
        import("package.tools.xmake").install(package)
    end)
end)
```

这是因为在编译命令中有大量相对于当前 `os.projectdir()` 的相对路径，导致编译的时候计算 cache-key 没对上。

我们将 local package 的 builddir 迁移到它项目里的 `build` 目录下，这样就可以保证相对路径统一从而可以复用缓存。

## 清理 xmake 多余的逻辑

* 清理标记 deprecated 的历史版本包袱
* 清理 windows 的逻辑

## 重写 core 下的 C++ 代码，力求精简

* 考虑先替换掉 Lua 成 5.4.6
* 需要足够多的测试用例
