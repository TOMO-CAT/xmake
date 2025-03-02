# follow

## 分化 commit

```bash
commit 71ec32ea6787c1c4b215ea40e1d5ab40c1c4eedc (HEAD, tag: v2.9.4, tag: v2.9.3)
Author: ruki <waruqi@gmail.com>
Date:   Wed Jul 24 23:30:07 2024 +0800

    update changelog
```

## Update to v2.9.5

* [#4760](https://github.com/xmake-io/xmake/issues/4760): 修复分布式构建报错
* [#5462](https://github.com/xmake-io/xmake/pull/5462): 添加 `xmake l cli.bisect`
* [#5488](https://github.com/xmake-io/xmake/pull/5488): 支持使用 cosmocc 去构建 xmake 自身二进制
* [#5491](https://github.com/xmake-io/xmake/pull/5491): 支持提供内嵌 lua 文件的单个 xmake 二进制文件
* [#4750](https://github.com/xmake-io/xmake/issues/4750): 修复 compile_commands 生成器，支持 `xmake tests`

## Update to v2.9.6

* [#5649](https://github.com/xmake-io/xmake/pull/5649): 添加 `package.check_fcsnippets`
* [#5631](https://github.com/xmake-io/xmake/pull/5631): 为 `add_linkgroups` 添加 `as_needed`
* [#5702](https://github.com/xmake-io/xmake/issues/5702): 改进 hash 模块
* [#5688](https://github.com/xmake-io/xmake/pull/5688): 改进 hashset
