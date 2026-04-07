# x86-32 汇编大作业 - 团队分工方案

> 平台：Linux x86-32 (IA-32)  
> 汇编器：NASM  
> 调用约定：cdecl（参数栈传参）

---

## 一、x86-32 特点提醒

### 寄存器紧张（只有8个通用）
```
EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP
```
- **EBP**：栈帧基址（函数内固定，别乱动）
- **ESP**：栈指针（push/pop会改变）
- **EAX**：通常用于返回值
- **ESI/EDI**：字符串操作专用

### 函数调用约定（cdecl）
```asm
; 参数从右到左压栈
push arg3
push arg2
push arg1
call func
add esp, 12         ; 调用者清理栈

; 返回值在 EAX
```

### 栈帧结构
```asm
my_func:
    push ebp
    mov ebp, esp
    sub esp, 16         ; 分配局部变量
    
    ; [ebp+8]  = 第1个参数
    ; [ebp+12] = 第2个参数
    ; [ebp-4]  = 局部变量1
    ; [ebp-8]  = 局部变量2
    
    mov esp, ebp
    pop ebp
    ret
```

---

## 二、团队分工详情

### 👤 成员A：项目负责人 + 主控模块

**性格要求**：细心、有耐心处理整合问题

**负责文件：**
| 文件 | 功能 | 预估代码量 |
|------|------|-----------|
| `main.asm` | 程序入口、主流程控制 | ~80行 |
| `file_io.asm` | 文件打开、读取、关闭 | ~60行 |
| `data.asm` | 数据段定义（全局变量、缓冲区） | ~40行 |
| `Makefile` | 编译脚本 | ~30行 |

**核心任务：**
1. **定义全局数据结构**
   ```asm
   ; data.asm 中定义
   MAX_WORDS equ 1000
   MAX_WORD_LEN equ 50
   WORD_ENTRY_SIZE equ 56    ; 50+4+2对齐
   
   section .bss
       word_table  resb MAX_WORDS * WORD_ENTRY_SIZE
       word_count  resd 1
       line_buffer resb 1024
       word_buffer resb 50
       fd          resd 1
   ```

2. **实现主函数流程**
   ```asm
   _start:
       call open_file
       call read_loop        ; 循环读取每行
       call find_most_frequent
       call print_result
       call sys_exit
   ```

3. **文件操作系统调用**
   ```asm
   ; sys_open, sys_read, sys_close (int 0x80)
   ; eax = 系统调用号
   ; ebx, ecx, edx = 参数
   ```

4. **整合所有模块**，解决符号冲突

**交付标准：**
- [ ] 能成功编译链接
- [ ] 能打开并读取文件
- [ ] 整合后程序能跑通基本流程

---

### 👤 成员B：数据处理核心（最复杂）

**性格要求**：逻辑思维强，善于处理循环和边界条件

**负责文件：**
| 文件 | 功能 | 预估代码量 |
|------|------|-----------|
| `parser.asm` | 文本解析、单词提取 | ~100行 |
| `word_table.asm` | 单词表查找、添加、更新 | ~80行 |

**核心任务：**

#### 1. processLine - 解析一行文本
```asm
; 参数: [ebp+8] = line_buffer地址
; 功能: 提取所有单词，调用add_or_update
process_line:
    push ebp
    mov ebp, esp
    sub esp, 8          ; 局部变量: i, j
    
    ; 伪代码:
    ; i = 0
    ; while line[i] != 0:
    ;     while is_delimiter(line[i]): i++
    ;     j = 0
    ;     while !is_delimiter(line[i]):
    ;         word[j++] = line[i++]
    ;     if j > 0:
    ;         word[j] = 0
    ;         call to_lower_case
    ;         call add_or_update_word
    
    mov esp, ebp
    pop ebp
    ret
```

#### 2. isDelimiter - 判断分隔符
```asm
; 参数: al = 字符
; 返回: ZF=1表示是分隔符
is_delimiter:
    ; 检查是否为字母 'a'-'z' 或 'A'-'Z'
    ; 是字母: 返回ZF=0
    ; 非字母: 返回ZF=1
```

#### 3. toLowerCase - 转小写
```asm
; 参数: [ebp+8] = 字符串地址
to_lower_case:
    ; 遍历字符串
    ; if 'A' <= c <= 'Z': c = c + 32
```

#### 4. findWord - 在单词表中查找
```asm
; 参数: [ebp+8]=table, [ebp+12]=n, [ebp+16]=word
; 返回: eax=索引(>=0) 或 -1
find_word:
    ; for i = 0 to n-1:
    ;     if strcmp(table[i].word, word) == 0:
    ;         return i
    ; return -1
```

#### 5. addOrUpdateWord - 添加或更新
```asm
; 参数: [ebp+8]=table, [ebp+12]=&count, [ebp+16]=word
add_or_update_word:
    ; idx = find_word(...)
    ; if idx >= 0:
    ;     table[idx].count++
    ; else:
    ;     strcpy(table[count].word, word)
    ;     table[count].count = 1
    ;     count++
```

**关键难点：**
- 数组索引计算：`table[i]` 的地址 = `table + i * WORD_ENTRY_SIZE`
- 结构体成员访问：`table[i].count` 的地址 = `table[i] + MAX_WORD_LEN`
- 寄存器分配紧张，需要频繁使用栈保存中间结果

**交付标准：**
- [ ] `process_line` 能正确分割单词
- [ ] `find_word` 能找到已存在的单词
- [ ] `add_or_update` 能正确添加新单词和更新计数

---

### 👤 成员C：字符串工具 + 输出

**性格要求**：注重细节，熟悉字符串操作

**负责文件：**
| 文件 | 功能 | 预估代码量 |
|------|------|-----------|
| `string.asm` | 字符串比较、复制、长度 | ~60行 |
| `output.asm` | 输出函数、格式化打印 | ~80行 |

**核心任务：**

#### 1. strcmp - 字符串比较
```asm
; 参数: [ebp+8]=s1, [ebp+12]=s2
; 返回: eax = 0(相等), >0(s1>s2), <0(s1<s2)
strcmp:
    push ebp
    mov ebp, esp
    push esi
    push edi
    
    mov esi, [ebp+8]    ; s1
    mov edi, [ebp+12]   ; s2
    
    ; 使用 repe cmpsb 指令
    ; 或手动循环比较
    
    pop edi
    pop esi
    pop ebp
    ret
```

#### 2. strcpy - 字符串复制
```asm
; 参数: [ebp+8]=dest, [ebp+12]=src
; 返回: eax = dest
strcpy:
    ; 使用 movsb 指令
    ; 或手动循环复制
```

#### 3. findMostFrequent - 找最高频
```asm
; 参数: [ebp+8]=table, [ebp+12]=n
;       [ebp+16]=result_word_buf, [ebp+20]=&result_count
find_most_frequent:
    ; max_idx = 0
    ; for i = 1 to n-1:
    ;     if table[i].count > table[max_idx].count:
    ;         max_idx = i
    ; strcpy(result_word, table[max_idx].word)
    ; *result_count = table[max_idx].count
```

#### 4. printResult - 输出结果
```asm
; 使用 sys_write (eax=4) 输出字符串
; 或使用 C 库的 printf（如果链接libc）

; 简单实现:
print_string:
    ; eax = 4 (sys_write)
    ; ebx = 1 (stdout)
    ; ecx = 字符串地址
    ; edx = 长度
    ; int 0x80
```

**交付标准：**
- [ ] `strcmp` 能正确比较字符串
- [ ] `strcpy` 能正确复制字符串（包括结束符）
- [ ] `find_most_frequent` 能找到正确的最高频单词
- [ ] 能正确输出结果到屏幕

---

### 👤 成员D：测试 + 文档

**性格要求**：细心、有耐心、善于发现问题

**负责文件：**
| 文件 | 内容 |
|------|------|
| `tests/test1.txt` ~ `test8.txt` | 各类测试数据 |
| `docs/design.md` | 设计文档 |
| `docs/api.md` | 接口说明 |
| `docs/test_report.md` | 测试报告 |
| `README.md` | 项目说明 |

**测试文件清单：**

| 文件名 | 测试场景 | 预期行为 |
|--------|---------|---------|
| `test1.txt` | 正常文本（多行，多种单词） | 正确统计频率 |
| `test2.txt` | 空文件 | 输出"无单词"提示 |
| `test3.txt` | 只有空格和换行 | 输出"无单词"提示 |
| `test4.txt` | 超长单词（>50字符） | 截断处理，不崩溃 |
| `test5.txt` | 大小写混合（Hello/HELLO/hello） | 统一统计为hello |
| `test6.txt` | 标点符号密集 | 正确分割单词 |
| `test7.txt` | 大量重复单词 | 计数正确，不溢出 |
| `test8.txt` | 接近MAX_WORDS上限 | 正常处理或提示满 |

**测试方法：**
```bash
# 编译
nasm -f elf32 main.asm -o main.o
nasm -f elf32 parser.asm -o parser.o
# ... 其他文件
ld -m elf_i386 *.o -o word_count

# 运行测试
./word_count test1.txt
./word_count test2.txt
# ...
```

**文档要求：**
1. **design.md**：程序架构、数据结构设计、算法流程图
2. **api.md**：每个函数的接口说明（参数、返回值、功能）
3. **test_report.md**：测试用例、测试结果、发现的bug及修复

**交付标准：**
- [ ] 8个测试文件全部创建
- [ ] 每个测试文件都有预期结果说明
- [ ] 测试报告记录完整
- [ ] README包含编译和运行说明

---

## 三、协作接口约定

### 全局符号（成员A定义，所有人使用）

```asm
; data.asm 中定义，其他文件用 extern 声明
section .data
    MAX_WORDS equ 1000
    MAX_WORD_LEN equ 50
    WORD_ENTRY_SIZE equ 56

section .bss
    global word_table
    global word_count
    global line_buffer
    global word_buffer
    
    word_table  resb MAX_WORDS * WORD_ENTRY_SIZE
    word_count  resd 1
    line_buffer resb 1024
    word_buffer resb 50
```

### 函数接口规范

| 函数 | 文件 | 参数（栈顺序） | 返回值 |
|------|------|---------------|--------|
| `open_file` | file_io.asm | filename | eax=fd或-1 |
| `read_line` | file_io.asm | fd, buf, size | eax=读取字节数 |
| `process_line` | parser.asm | line_buf | 无 |
| `is_delimiter` | parser.asm | char(al) | ZF |
| `to_lower_case` | parser.asm | str | 无 |
| `find_word` | word_table.asm | table, n, word | eax=索引/-1 |
| `add_or_update` | word_table.asm | table, &count, word | 无 |
| `strcmp` | string.asm | s1, s2 | eax=差值 |
| `strcpy` | string.asm | dest, src | eax=dest |
| `find_most_frequent` | output.asm | table, n, result, &count | 无 |
| `print_result` | output.asm | 无 | 无 |

---

## 四、开发时间表

| 周次 | 任务 | 交付物 | 检查点 |
|------|------|--------|--------|
| **第1周** | 搭建框架 | 各模块空函数，能编译 | 每人提交自己的框架代码 |
| **第2周** | 核心开发 | 各模块功能实现 | 代码审查（互相检查） |
| **第3周** | 首次整合 | 能跑通基本流程 | 成员A整合，全员测试 |
| **第4周** | Bug修复 | 修复已知问题 | 成员D提供bug列表 |
| **第5周** | 全面测试 | 通过全部测试用例 | 成员D主导测试 |
| **第6周** | 文档完善 | 完整文档 + 演示准备 | 全员参与 |

---

## 五、Git协作流程

### 分支命名规范
```
main                    # 主分支，稳定代码
├── zhangsan/main       # 成员A的主控模块
├── lisi/parser         # 成员B的解析模块
├── wangwu/string       # 成员C的字符串模块
└── zhaoliu/tests       # 成员D的测试文档
```

### 提交信息规范
```
[模块] 具体修改内容

示例:
[parser] 实现 process_line 函数，支持空格分割
[string] 修复 strcmp 空指针bug
[tests] 添加 test4.txt 超长单词测试
[docs] 更新 API 接口说明
```

### 每周检查流程
1. **周五晚**：所有人提交本周代码到自己的分支
2. **周六**：成员A尝试合并到main，记录冲突
3. **周日**：全员讨论解决冲突，确定下周计划

---

## 六、常见问题预案

### Q1: 寄存器不够用怎么办？
**解决**：优先使用EBX/ESI/EDI保存长期值，临时值压栈保存
```asm
push eax            ; 保存eax
; ... 使用eax做其他事 ...
pop eax             ; 恢复eax
```

### Q2: 怎么调试汇编程序？
**解决**：使用GDB单步调试
```bash
nasm -f elf32 -g main.asm -o main.o    # -g 生成调试信息
ld -m elf_i386 -g *.o -o word_count
gdb ./word_count
(gdb) break _start
(gdb) run
(gdb) stepi           # 单步执行
(gdb) info registers  # 查看寄存器
(gdb) x/10x $esp      # 查看栈内容
```

### Q3: 系统调用返回值怎么判断错误？
**解决**：Linux系统调用返回负数表示错误
```asm
int 0x80
cmp eax, 0
jl error_handler      ; 如果eax < 0，出错
```

### Q4: 字符串长度怎么计算？
**解决**：使用 scasb 指令或手动循环
```asm
; 手动实现 strlen
strlen:
    mov ecx, -1         ; 最大计数
    xor al, al          ; 查找 '\0'
    repne scasb         ; 扫描直到找到al
    ; 结果是 -ecx - 1
```

---

## 七、参考资源

- [NASM文档](https://www.nasm.us/doc/)
- [x86汇编指南](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html)
- [Linux系统调用表](https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md)
- [cdecl调用约定](https://en.wikipedia.org/wiki/X86_calling_conventions#cdecl)

---

**祝团队协作顺利！** 🚀
