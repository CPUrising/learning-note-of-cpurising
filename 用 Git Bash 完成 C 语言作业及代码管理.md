# 用 Git Bash 完成 C 语言作业及代码管理

非常合适！用 Git Bash 来完成 “传 C 语言作业到 GitHub” 以及 “管理刷题 / 课堂代码 / 读书笔记” 这些操作，甚至比用 PowerShell 更贴合场景，核心原因是 Git Bash 对 **Git 命令的原生适配** 和 **Linux 风格文件操作的兼容性**，能让整个流程更顺畅。下面结合你提到的具体例子，拆解 Git Bash 的适配优势和实际操作细节：

### 一、为什么用 Git Bash 做这些事更合适？



1. **Git 命令 “零配置” 可用**
   Git Bash 是 Git 安装包自带的工具，打开后不需要额外设置环境变量，就能直接敲 `git init`、`git add`、`git push` 等命令 —— 不像早期 PowerShell 可能需要手动配置 Git 路径才能识别命令（虽然现在新版 PowerShell 也能自动识别，但 Git Bash 从根源上避免了兼容性问题）。

2. **文件路径操作更贴近代码管理习惯**
   你例子中提到 “进入 C-homework 文件夹”，用 Git Bash 可以直接用 **Linux 风格的路径命令**（和 macOS 终端一致），比如：
* 查看当前文件夹内容：`ls`（比 PowerShell 的 `dir` 更符合程序员跨系统操作习惯）；

* 切换到 C 盘的作业文件夹：`cd /c/Users/你的名字/Documents/C-homework`（注意 Git Bash 中 Windows 的 “C 盘” 会被识别为 `/c`，路径用 `/` 分隔，不用记 PowerShell 中 `C:\Users\...` 的反斜杠 `\`，减少输入错误）；

* 删除多余文件：`rm 无用文件.c`（和后续 Linux 服务器操作逻辑一致，提前养成习惯）。
1. **支持 Markdown 相关工具（进阶场景适配）**
   后续管理 “读书笔记 / 课件” 时，可能需要用命令行预览 Markdown（比如 `cat ``README.md` 查看内容）、甚至用简单工具转换格式，Git Bash 对这些轻量命令的支持更自然，不需要额外安装插件。

### 二、用 Git Bash 完成 “传 C 语言作业” 的实际步骤（和原流程几乎一致，仅路径写法微调）

假设你的 `C-homework` 文件夹在 Windows 的 **C 盘 → 用户 → 你的名字 → 文档** 里，用 Git Bash 操作的具体步骤：



1. **打开 Git Bash**：从开始菜单找到 “Git Bash”（图标是黑色背景带白色 `git` 字样），双击打开；

2. **进入作业文件夹**：敲以下命令（替换 “你的名字” 为实际用户名），按回车：
   `cd /c/Users/你的名字/Documents/C-homework`
   ✅ 提示：敲到 `cd /c/Users/` 后，按 `Tab` 键可以自动补全用户名，减少输入；

3. **后续步骤和原流程完全一样**：
* `git init`（标记文件夹为 Git 仓库）；

* `git add .`（添加所有文件到暂存区）；

* `git commit -m "2024.9.24 C语言作业：求素数"`（提交并写备注）；

* 复制 GitHub 仓库链接，执行 `git remote add origin 你的仓库链接`；

* `git push origin master`（推送到 GitHub）。

### 三、管理 “刷题 / 课堂代码 / 读书笔记” 时，Git Bash 的额外便利

比如你要建 “LeetCode-Practice” 刷题仓库，用 Git Bash 可以快速完成文件夹创建和初始化：



1. 新建刷题文件夹并进入：
   `mkdir LeetCode-Practice && cd LeetCode-Practice`（`mkdir` 是 “创建文件夹” 命令，`&&` 表示 “先执行前面的命令，成功后再执行后面的”，一步到位）；

2. 初始化仓库并创建 README.md：
   `git init && touch ``README.md`（`touch` 是 “创建空文件” 命令，直接生成 Markdown 笔记文件，比在图形界面右键新建更高效）；

3. 后续添加题目文件夹（比如 “001 - 两数之和”）：
   `mkdir 001-两数之和 && cd 001-两数之和 && touch solution.c`（一次性完成 “建文件夹→进文件夹→建代码文件”）。

### 总结

用 Git Bash 做这些事，不仅完全能覆盖需求，还能帮你提前适应 **跨系统的命令行操作逻辑**（比如以后用 Linux 服务器管理代码时，命令和 Git Bash 几乎一致），属于 “现在多练一点，以后少踩坑” 的选择。而且操作步骤和你之前了解的流程高度重合，只需要微调路径写法，上手成本很低。



## 新代码如何更新到github

当你写了新代码（比如在已有的 `C-homework` 仓库里新增了 `sort.c` 排序作业），更新到 GitHub 的流程比首次上传更简单，只需要 3 步核心操作（基于 Git Bash 环境）：

##### **具体步骤（以新增代码为例）**假设你已经在本地仓库（比如 `C-homework` 文件夹）里写了新代码，现在要同步到 GitHub：

##### 1. **查看当前修改状态（可选但推荐）**

敲命令： ```bash git status ``` 

作用：查看哪些文件被修改或新增了（会显示 `untracked files` 或 `modified` 标记），确认你要上传的代码是否在列表中（比如看到 `sort.c` 是新增文件）。

##### 2. **把新代码“放进盒子”并“贴备注”** 分两步：

```bash # 第一步：添加所有修改/新增的文件（. 表示当前文件夹下所有内容） git add . # 第二步：提交并写备注（说明这次更新了什么，比如“新增排序作业”） git commit -m "2024.9.25 新增排序作业：冒泡排序实现" ``` 
# 第一步：添加所有修改/新增的文件（. 表示当前文件夹下所有内容）
git add .

# 第二步：提交并写备注（说明这次更新了什么，比如“新增排序作业”）
git commit -m "2024.9.25 新增排序作业：冒泡排序实现"
```

✅ 提示：如果只想上传单个文件（比如只传 `sort.c`），可以把 `git add .` 换成 `git add sort.c`。3. **推送到 GitHub（更新远程仓库）** 

敲命令： ```bash git push origin master ``` 

（如果你的主分支叫 `main` 而不是 `master`，就敲 `git push origin main`，可以在 GitHub 仓库页面看分支名称） 执行后，Git 会把本地新增的代码同步到 GitHub，刷新仓库页面就能看到新文件了。



### **总结：更新代码的核心逻辑**和首次上传相比，少了 `git init`（仓库已初始化）和 `git remote add`（已关联远程地址），只需要： `git add .` → `git commit -m "备注"` → `git push origin 分支名` 每次写完新代码都重复这三步，就能让本地和 GitHub 仓库始终保持同步，避免代码丢失或版本混乱。
