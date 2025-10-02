# 新代码如何更新到 GitHub

当你写了新代码（比如在已有的 `C-homework` 仓库里新增了 `sort.c` 排序作业），更新到 GitHub 的流程比首次上传更简单，只需要 3 步核心操作（基于 Git Bash 环境）：

## 具体步骤（以新增代码为例）

假设你已经在本地仓库（比如 `C-homework` 文件夹）里写了新代码，现在要同步到 GitHub：

### 0. 进入主分支与目标目录



```git
cd /d/documents/课本笔记/temp/
```

### 1. 查看当前修改状态（可选但推荐）

敲命令：



```git
git status
```

作用：查看哪些文件被修改或新增了（会显示 `untracked files` 或 `modified` 标记），确认你要上传的代码是否在列表中（比如看到 `sort.c` 是新增文件）。

### 2. 把新代码 “放进盒子” 并 “贴备注”

分两步执行：



```
\# 第一步：添加所有修改/新增的文件（. 表示当前文件夹下所有内容）

git add .

\# 第二步：提交并写备注（说明这次更新了什么，比如“新增排序作业”）

git commit -m "2024.9.25 新增排序作业：冒泡排序实现"
```

✅ 提示：如果只想上传单个文件（比如只传 `sort.c`），可以把 `git add .` 换成 `git add sort.c`。

### 3. 推送到 GitHub（更新远程仓库）

敲命令：



```
git push origin main
```

（如果你的主分支叫 `main` 而不是 `master`，就敲 `git push origin main`，可以在 GitHub 仓库页面看分支名称）

执行后，Git 会把本地新增的代码同步到 GitHub，刷新仓库页面就能看到新文件了。

## 总结：更新代码的核心逻辑

和首次上传相比，少了 `git init`（仓库已初始化）和 `git remote add`（已关联远程地址），只需要：

`git add .` → `git commit -m "备注"` → `git push origin 分支名`

每次写完新代码都重复这三步，就能让本地和 GitHub 仓库始终保持同步，避免代码丢失或版本混乱。



```
git push origin main
```

> （注：文档部分内容可能由 AI 生成）
