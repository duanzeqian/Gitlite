# Project 4: My solution of Gitlite

## 项目骨架

```text
gitlite/
├── include/                        # 头文件
│   ├── Commands/                   # 各种指令，文件名对应指令名
│   │   ├── Init.h
│   │   ├── Add.h
│   │   ├── CommitCmd.h
│   │   ├── Rm.h
│   │   ├── Log.h
│   │   ├── GlobalLog.h
│   │   ├── Find.h
│   │   ├── Status.h
│   │   ├── Checkout.h
│   │   ├── Branch.h
│   │   ├── RmBranch.h
│   │   ├── Reset.h
│   │   ├── Merge.h
│   │   ├── AddRemote.h
│   │   ├── RmRemote.h
│   │   ├── Fetch.h
│   │   ├── Push.h
│   │   └── Pull.h
│   ├── Utils.h
│   ├── GitliteException.h
│   ├── Repository.h                # Gitlite的核心数据管理类
│   ├── GitliteObject.h             # 所有object的基类，下面三个文件为三种object
│   ├── Blob.h
│   ├── Tree.h
│   ├── Commit.h
│   ├── Remote.h                    # 远程仓库的类，用于在std::map中存储某个远程仓库的信息
│   └── SomeObj.h                   # 对所有指令的上层封装，以便于在main函数中直接调用指令
├── src/                            # 源代码
│   ├── Commands/
│   │   ├── Init.cpp
│   │   ├── Add.cpp
│   │   ├── CommitCmd.cpp
│   │   ├── Rm.cpp
│   │   ├── Log.cpp
│   │   ├── GlobalLog.cpp
│   │   ├── Find.cpp
│   │   ├── Status.cpp
│   │   ├── Checkout.cpp
│   │   ├── Branch.cpp
│   │   ├── RmBranch.cpp
│   │   ├── Reset.cpp
│   │   ├── Merge.cpp
│   │   ├── AddRemote.cpp
│   │   ├── RmRemote.cpp
│   │   ├── Fetch.cpp
│   │   ├── Push.cpp
│   │   └── Pull.cpp
│   ├── Utils.cpp
│   ├── GitliteException.cpp
│   ├── Repository.cpp
│   ├── GitliteObject.cpp
│   ├── Blob.cpp
│   ├── Tree.cpp
│   ├── Commit.cpp
│   ├── Remote.cpp
│   └── SomeObj.cpp
├── testing/                        # 测试文件
│   ├── samples/
│   ├── src/
│   ├── tester.py
│   ├── out.txt
│   └── MakeFile
├── main.cpp
├── CMakeLists.txt                  # 项目编译和运行
└── README.md
```

## 实现思路

### 1. 核心类设计

#### GitliteObject（基类）

**作用**：所有持久化对象的基类（Blob、Tree、Commit）。

**实例变量：**

`hash`：对象的 SHA-1 哈希值。

**核心方法：**

`serialize()`：将对象序列化为字节流。

`deserialize()`：从字节流反序列化对象。

`computeHash()`：计算对象的哈希值。

**工作原理**：通过虚函数实现多态，不同子类实现各自的序列化逻辑。


#### Blob（文件内容对象）

**作用**：存储文件的具体内容。

**实例变量**：

`content`：文件的二进制内容。

**序列化格式**：`字节数 字节1 字节2 ...`。

**持久化**：存储在 .gitlite/objects/xx/xxxxxxxx... 中，文件夹名与文件名组合起来为其哈希值。


#### Tree（目录树对象）
**作用**：记录文件名到 Blob 哈希的映射（简化版，不处理子目录）。

**实例变量**：

`entries`：从文件名到Blob哈希值的映射。

**序列化格式**：`文件数 文件名1 哈希1 文件名2 哈希2 ...`。

**工作原理**：每次提交时基于当前工作树和暂存区生成新的 Tree 对象。


#### Commit（提交对象）
**作用**：记录一次提交的元数据。

**实例变量**：

`treeHash`：对应 Tree 对象的哈希。

`fatherHashes`：父提交哈希数组（支持合并提交）。

`message`：提交信息。

`timestamp`：提交时间戳。

**序列化格式**：多行文本，包含树哈希、父提交数、父提交哈希、时间戳、信息。


#### Repository（仓库管理类）
**作用**：整个版本控制系统的核心，管理所有状态和操作。

**实例变量**：

`workTree`：工作目录路径。

`gitDir`：.gitlite 目录路径。

`branches`：从分支名到commit哈希的映射。

`stagingArea`：暂存区，我用一个Tree对象来管理暂存区文件。

`rmFiles`：待删除文件集合。

`remotes`：远程仓库映射，映射到的为Remote类，存储某一个远程仓库的基本信息。

**核心功能**：

对象存储与读取（`storeObject()` / `readObject()`）。

分支管理（`createBranch()` / `eleteBranch()`）。

暂存区操作（`stageFile()` / `unstageFile()`）。

提交创建（`createCommit()` / `createCommitInMerge()`）。

远程操作支持（`copyCommitHistory()` / `copyObject()`）。

分割点查找： BFS找LCA


#### Remote（远程仓库信息）
**作用**：存储单个远程仓库的信息。

**实例变量**：

`name`：远程仓库名称。

`path`：远程仓库路径（本地文件系统路径）。


#### SomeObj（指令封装类）
**作用**：对 Commands/ 下的所有指令类进行统一封装，便于 main.cpp 调用。

**实例变量**：

`repo`：Repository 的引用。

**方法**：对应每个 Gitlite 命令的公有方法，内部创建对应指令对象并执行。



### 2. 持久化实现

#### .gitlite骨架

```text

.gitlite/
├── HEAD                        # 当前分支引用
├── index                       # 暂存区序列化数据
├── remove                      # 待删除文件列表
├── objects/                    # 对象存储
│   ├── xx/                     # 哈希前两位作为目录名
│   │   └── xxxxxx...           # 对象文件（哈希剩余部分）
├── refs/
│   └── heads/                  # 本地分支
│       ├── master              # 初始分支
│       └── ...                 # 其它分支
│
└── remotes/                    # 远程仓库配置（在remote相关指令中再创建）
    └── remote_name             # 远程仓库路径，以远程仓库名为文件名

```

通过将修改的信息及时磁盘化，实现操作的持久化。

#### 序列化与反序列化

**对象存储格式**：`类型 大小\0内容`

**哈希计算**：SHA-1(`类型 + " " + 大小 + '\0' + 内容`)。

**索引与状态**：

`index`：存储`stagingArea`的序列化数据。

`remove`：每行一个待删除文件名。

`HEAD`：`ref: refs/heads/分支名`或直接指向提交哈希。

**对象读取流程**：

1.根据哈希前两位确定子目录。

2.读取文件，解析头部获取类型和大小。

3.根据类型创建对应子类对象，调用对应的`deserialize()`。


### 3. 类的工作原理

#### 边界情况

感觉所有边界情况都在下发的`README`里写好了啊（），除了在远程操作中要注意branch的名字可能会带`/`的问题。

**解决方案**：在存储时将`/`替换为`~`，读取时反向替换。

`encodeBranchName()`：remoteName/remoteBranchName → remoteName~remoteBranchName

`decodeBranchName()`：remoteName~remoteBranchName → remoteName/remoteBranchName

当然这种解决方案也可以应用到branchName本身就带`/`的情况，只需要在`SomeObj.cpp`的实现中在`execute`之前进行转义就行了。

~~不过根据面向数据点编程的原理，本项目只对branchName进行了转义考虑，转义考虑也仅仅考虑了‘/’这一种字符。~~

#### 复杂任务确定方法

说实话，这个项目好像确实没什么复杂任务（）~~这波我支持wqy助教~~，最多就是`merge`函数需要稍微思考一下8种合并逻辑。

**解决方案**：

首先通过BFS找LCA，一共获得三个点的Tree，标记为`LCATree`，`currentTree`，`givenTree`。

然后我们将这些Tree里的所有文件打包丢到一个vector里，这些文件就是我们可能需要操作的文件。

接下来遍历所有文件，对每一个`fileName`，我们用`inLCA`，`inCurrent`，`inGiven`记录它们是否在对应的Tree当中，`LCAContent`，`currentContent`，`givenContent`记录它们的内容，用于比较内容有没有修改。

最后就是把`README`中的八种case分别用上述变量表示出来就行了。

当我们遇到合并冲突时，先生成冲突文件的内容：

```text
std::stringstream conflictContent;
conflictContent << "<<<<<<< HEAD\n";
conflictContent << currentContent;
conflictContent << "=======\n";
conflictContent << givenContent;
conflictContent << ">>>>>>>\n";
```

再把它写到工作目录中，最后丢到暂存区就行了。~~咱也不知道什么叫“如何确定其算法”，这里面有什么算法吗？~~


### 4.总结

这个项目的框架在于在`Utils`和`Repository`中实现基础功能和磁盘化操作，在各种commands中直接调用相关功能而非现写，在`SomeObj`中对所有commands再进行上层封装，并与`main.cpp`相联系，最后在`main.cpp`中对输入指令（或者乱码）的判断和分类，就能有层次地实现Gitlite的所有基本功能了。

感觉这个`README`写到这里就差不多了，这个项目的核心难点就在于设计和封装~~start from Scratch~~。援引伟大的gwb学长原话来说就是“gitlite是教你设计和封装的”“我说白了，这个大作业是教你OOP的，类不是用来简化代码的，是有核心思想在的，为什么一定要这个抽象层”。

~~（龚✌✌的恩情还不完）~~


球球捞分[可怜][可怜][可怜]