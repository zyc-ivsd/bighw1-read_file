# 汇编大作业 - 团队协作指南

> 本仓库用于协作完成"单词频率统计"RISC-V汇编程序

---

## 一、团队分工

### 成员职责总览

| 成员 | 主要职责 | 核心模块 | 预估代码量 |
|------|---------|---------|-----------|
| **成员A** | 项目负责人 | 主函数 + 文件IO | ~150行 |
| **成员B** | 数据处理 | 单词分割 + 查表统计 | ~180行 |
| **成员C** | 工具函数 | 字符串操作 + 输出 | ~140行 |
| **成员D** | 测试文档 | 测试用例 + 文档 | 5-10个测试文件 |

---

### 成员A：主控模块 + 文件操作

**负责文件：**
- `src/main.s` - 程序入口和主流程
- `src/file_io.s` - 文件打开、读取、关闭
- `src/data.s` - 数据段定义（全局变量、缓冲区）
- `Makefile` - 编译脚本

**核心任务：**
1. 定义全局数据结构和缓冲区
2. 实现 `main` 函数主流程
3. 实现文件操作（`fopen`, `fgets`, `fclose`）
4. 整合所有模块，确保能编译运行
5. 协调团队进度

**接口约定（需提供给其他成员）：**
```asm
# 全局数据（在 data.s 中定义）
.globl word_table          # 单词表起始地址
.globl word_count          # 当前单词数量
.globl line_buffer         # 行缓冲区（1024字节）
.globl word_buffer         # 单词缓冲区（50字节）

# 常量定义
.equ MAX_WORDS, 1000
.equ MAX_WORD_LEN, 50
.equ WORD_ENTRY_SIZE, 56   # 50字节word + 4字节count + 2字节对齐
```

---

### 成员B：数据处理核心

**负责文件：**
- `src/parser.s` - 文本解析和单词提取
- `src/word_table.s` - 单词表操作

**核心任务：**
1. 实现 `processLine` - 解析一行文本，提取所有单词
2. 实现 `isDelimiter` - 判断字符是否为分隔符
3. 实现 `toLowerCase` - 字符串转小写
4. 实现 `findWord` - 在单词表中查找单词
5. 实现 `addOrUpdateWord` - 添加新单词或更新计数

**关键算法提示：**
```c
// processLine 的C逻辑参考
void processLine(WordEntry *table, int *count, char *line) {
    int i = 0;
    while (line[i] != '\0') {
        // 跳过非字母
        while (isDelimiter(line[i])) i++;
        
        // 收集单词
        int j = 0;
        while (!isDelimiter(line[i]) && j < MAX_WORD_LEN-1) {
            word[j++] = line[i++];
        }
        
        if (j > 0) {
            word[j] = '\0';
            toLowerCase(word);
            addOrUpdateWord(table, count, word);
        }
    }
}
```

---

### 成员C：字符串工具 + 输出

**负责文件：**
- `src/string.s` - 字符串操作函数
- `src/output.s` - 输出和结果显示

**核心任务：**
1. 实现 `strcmp` - 字符串比较
2. 实现 `strcpy` - 字符串复制
3. 实现 `strlen` - 字符串长度（可选）
4. 实现 `findMostFrequent` - 找出最高频单词
5. 实现 `printAllWords` - 打印所有统计结果
6. 实现格式化输出辅助函数

**函数规范：**
```asm
# strcmp(a0=s1, a1=s2) -> a0=差值(0表示相等)
# strcpy(a0=dest, a1=src) -> a0=dest
# findMostFrequent(a0=table, a1=n, a2=resultWord, a3=&resultCount)
# printAllWords(a0=table, a1=n)
```

---

### 成员D：测试 + 文档

**负责文件：**
- `tests/test*.txt` - 各类测试文件
- `docs/design.md` - 设计文档
- `docs/api.md` - 接口说明文档
- `docs/test_report.md` - 测试报告
- `README.md` - 项目说明

**测试文件清单：**
| 文件名 | 测试内容 | 预期结果 |
|--------|---------|---------|
| `test1.txt` | 正常多行文本 | 正确统计各单词频率 |
| `test2.txt` | 空文件 | 输出"无单词" |
| `test3.txt` | 只有空格和换行 | 输出"无单词" |
| `test4.txt` | 超长单词（>50字符） | 截断处理，不崩溃 |
| `test5.txt` | 大小写混合（Hello/HELLO/hello） | 统一统计为hello |
| `test6.txt` | 密集标点符号 | 正确分割单词 |
| `test7.txt` | 大量重复单词 | 计数正确 |
| `test8.txt` | 接近MAX_WORDS上限 | 正常处理或提示满 |

**文档要求：**
- 每个函数添加详细注释（功能、参数、返回值）
- 记录测试过程和发现的bug
- 编写使用说明

---

## 二、Git协作流程

### 2.1 首次使用（每人执行一次）

```bash
# 1. 克隆仓库到本地
git clone https://github.com/你的用户名/bighw1-read_file.git
cd bighw1-read_file

# 2. 配置个人信息（重要！）
git config user.name "你的姓名"
git config user.email "你的邮箱"
```

---

### 2.2 日常开发流程

#### 步骤1：开始工作前，拉取最新代码

```bash
git checkout main
git pull origin main
```

#### 步骤2：创建自己的开发分支

```bash
# 命名规范：姓名/功能名
git checkout -b zhangsan/parser
# 或
git checkout -b lisi/string-utils
```

#### 步骤3：编写代码...

#### 步骤4：查看修改

```bash
# 查看哪些文件被修改
git status

# 查看具体修改内容
git diff src/parser.s
```

#### 步骤5：提交代码

```bash
# 添加修改的文件
git add src/parser.s

# 或添加所有修改
git add .

# 提交（写清晰的提交信息）
git commit -m "实现 processLine 函数，支持空格和标点分割"
```

**提交信息规范：**
- ✅ `实现 isDelimiter 函数，判断字母和分隔符`
- ✅ `修复 strcmp 空指针bug`
- ✅ `添加测试用例 test4.txt（超长单词）`
- ❌ `update`
- ❌ `fix bug`
- ❌ `111`

#### 步骤6：推送到GitHub

```bash
git push origin zhangsan/parser
```

---

### 2.3 合并代码（Pull Request）

#### 方式A：通过GitHub网页（推荐新手）

1. 打开 GitHub 仓库页面
2. 点击 **"Pull requests"** → **"New pull request"**
3. 选择：`base: main` ← `compare: 你的分支名`
4. 填写标题和描述：
   ```
   标题：实现单词解析模块
   
   描述：
   - 完成 processLine 函数
   - 完成 isDelimiter 函数  
   - 完成 toLowerCase 函数
   - 已通过本地测试
   ```
5. 点击 **"Create pull request"**
6. 通知队友审查，通过后点击 **"Merge"**

#### 方式B：命令行合并（适合简单情况）

```bash
# 1. 切换到main分支
git checkout main

# 2. 拉取最新代码
git pull origin main

# 3. 合并你的分支
git merge zhangsan/parser

# 4. 推送到GitHub
git push origin main
```

---

### 2.4 处理代码冲突

当多人修改同一文件时，可能产生冲突。

```bash
# 1. 拉取最新代码时提示冲突
git pull origin main
# Auto-merging src/main.s
# CONFLICT (content): Merge conflict in src/main.s

# 2. 打开冲突文件，找到冲突标记
<<<<<<< HEAD
    # 你的代码
=======
    # 队友的代码
>>>>>>> main

# 3. 手动编辑，保留正确代码，删除冲突标记

# 4. 重新提交
git add src/main.s
git commit -m "解决main.s合并冲突"
git push origin main
```

---

### 2.5 常用Git命令速查

| 命令 | 作用 |
|------|------|
| `git status` | 查看当前状态 |
| `git log --oneline` | 查看提交历史（简洁） |
| `git log --graph` | 查看分支图 |
| `git branch` | 查看本地分支 |
| `git branch -a` | 查看所有分支 |
| `git checkout 分支名` | 切换分支 |
| `git checkout -b 新分支` | 创建并切换分支 |
| `git branch -d 分支名` | 删除本地分支 |
| `git push origin --delete 分支名` | 删除远程分支 |
| `git stash` | 暂存当前修改 |
| `git stash pop` | 恢复暂存的修改 |

---

## 三、文件命名和代码规范

### 3.1 文件命名

```
src/
├── main.s          # 主程序
├── file_io.s       # 文件操作
├── parser.s        # 文本解析
├── word_table.s    # 单词表管理
├── string.s        # 字符串工具
├── output.s        # 输出函数
└── common.inc      # 公共定义（头文件）

tests/
├── test1.txt       # 正常测试
├── test2.txt       # 空文件测试
└── ...

docs/
├── design.md       # 设计文档
├── api.md          # 接口文档
└── test_report.md  # 测试报告
```

### 3.2 汇编代码规范

```asm
# ============================================================================
# 文件: parser.s
# 作者: 张三
# 功能: 文本解析和单词提取
# 日期: 2024-04-07
# ============================================================================

    .text
    .globl processLine
    .globl isDelimiter
    .globl toLowerCase

# ============================================================================
# 函数: processLine
# 描述: 解析一行文本，提取所有单词并统计
# 参数:
#   a0 - word_table 地址
#   a1 - word_count 地址
#   a2 - line_buffer 地址
# 返回: 无
# ============================================================================
processLine:
    # 函数序言
    addi  sp, sp, -48
    sd    ra, 40(sp)
    sd    s0, 32(sp)
    
    # 函数体...
    
    # 函数尾声
    ld    ra, 40(sp)
    ld    s0, 32(sp)
    addi  sp, sp, 48
    ret

# ============================================================================
# 函数: isDelimiter
# 描述: 判断字符是否为分隔符（非字母）
# 参数: a0 - 字符
# 返回: a0 - 1(是分隔符) 或 0(不是分隔符)
# ============================================================================
isDelimiter:
    # 实现...
    ret
```

---

## 四、开发时间表  (具体时间节点快点)

| 周次 | 任务 | 交付物 | 负责人 |
|------|------|--------|--------|
| **第1周** | 搭建框架 | 各模块空函数 + 能编译 | 全员 |
| **第2周** | 核心开发 | 各模块功能完成 | 全员 |
| **第3周** | 首次整合 | 能跑通基本流程 | 成员A |
| **第4周** | Bug修复 | 修复已知问题 | 全员 |
| **第5周** | 测试优化 | 通过全部测试用例 | 成员D主导 |
| **第6周** | 文档完善 | 完整文档 + 演示 | 全员 |

---


### Issue使用示例：

```
标题: [Bug] processLine 处理空行时崩溃
标签: bug, parser
指派: 成员B
描述:
- 测试文件: test2.txt（空文件）
- 现象: 程序崩溃
- 期望: 正常退出，提示无单词
```

---

## 六、常见问题

### Q1: 推送到GitHub时提示权限错误？

**解决：** 检查是否被添加为仓库协作者（Settings → Manage access）

### Q2: 忘记创建分支，直接在main上修改了？

```bash
# 保存修改
git stash

# 创建新分支
git checkout -b 新分支

# 恢复修改
git stash pop
```

### Q3: 代码写错了，想回退到上次提交？

```bash
# 查看提交历史
git log --oneline

# 回退到指定版本（谨慎使用！）
git reset --hard 提交ID

# 或只回退工作区，保留修改
git checkout -- 文件名
```

### Q4: 如何查看队友的代码？

```bash
# 拉取所有远程分支
git fetch origin

# 查看队友分支
git branch -r

# 切换到队友分支查看
git checkout origin/lisi/string-utils
```

---

## 七、参考资源

- [RISC-V 指令集手册](https://riscv.org/technical/specifications/)
- [RISC-V 汇编编程指南](https://github.com/riscv-non-isa/riscv-asm-manual)
- [Git 简明教程](https://rogerdudler.github.io/git-guide/index-zh.html)

---

**祝大家协作愉快，顺利完成大作业！** 🎉
