# Todo

## 自动补全功能

按这个 issue [#5259](https://github.com/xmake-io/xmake/issues/5259) 的描述和[A Tour of xmake](https://zhuanlan.zhihu.com/p/548734585)，xmake 有自动补全和虚拟环境的功能，但是源码安装的目前没有自动补全，后面再研究。

## build.fence policy 导致 cpu 利用率不高

当前假设 target A 依赖 target B，而 target B 是一个 proto / codegen target 导致 target A 必须依赖 target B 产出的 .h 文件才能编译。那么我们往往要给 target B 设置 `build.fence` policy 为 true，当前这个策略会导致 target A 的 `before_build` job 需要等待 target B 的 `link` job 运行完才能开始运行。

尽管这没什么必要，但是当前的 batchjobs 不太好修改。
