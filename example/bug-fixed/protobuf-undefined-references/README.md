# README

## issue

<https://github.com/xmake-io/xmake/issues/5426>

## 问题描述

using a rule with before_buildcmd_file inside a target with set_kind("object") that has another target with set_kind("object") as dependency, as well as using the rule in the main target results in the generated object file not being linked.
