#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAX_WORDS 1000      
#define MAX_WORD_LEN 50     
#define MAX_LINE_LEN 1024   
typedef struct {
    char word[MAX_WORD_LEN];  /* 单词内容 */
    int count;                /* 出现次数 */
} WordEntry;

/* 将字符串转换为小写（统一大小写统计） */
void toLowerCase(char *str);

/* 在单词表中查找单词，返回索引，未找到返回-1 */
int findWord(WordEntry table[], int n, const char *word);

/* 添加新单词或更新已有单词的计数 */
void addOrUpdateWord(WordEntry table[], int *n, const char *word);

/* 找出出现次数最多的单词 */
void findMostFrequent(WordEntry table[], int n, char *resultWord, int *resultCount);

/* 打印所有单词统计结果（调试用） */
void printAllWords(WordEntry table[], int n);

/* 判断字符是否为单词分隔符（空格、标点、换行等） */
int isDelimiter(char c);

/* 从一行文本中提取并处理单词 */
void processLine(WordEntry table[], int *wordCount, char *line);
int main(int argc, char *argv[]) {
    FILE *fp;
    char line[MAX_LINE_LEN];
    WordEntry wordTable[MAX_WORDS];
    int wordCount = 0;
    char mostFreqWord[MAX_WORD_LEN];
    int mostFreqCount = 0;
    const char *filename;
    
    /* 处理命令行参数 */
    if (argc > 1) {
        filename = argv[1];
    } else {
        filename = "input.txt";  /* 默认文件名 */
    }
    
    printf("=====================================\n");
    printf("      单词频率统计程序\n");
    printf("=====================================\n");
    printf("正在读取文件: %s\n\n", filename);
    
    /* 1. 打开文件 */
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("错误：无法打开文件 '%s'\n", filename);
        printf("请确保文件存在，或使用命令行参数指定文件路径\n");
        printf("用法: %s <文件名>\n", argv[0]);
        return 1;
    }
    
    /* 2. 逐行读取并处理 */
    while (fgets(line, sizeof(line), fp) != NULL) {
        processLine(wordTable, &wordCount, line);
    }
    
    /* 3. 关闭文件 */
    fclose(fp);
    
    /* 4. 检查是否读取到单词 */
    if (wordCount == 0) {
        printf("文件中没有找到任何单词\n");
        return 0;
    }
    
    /* 5. 找出出现次数最多的单词 */
    findMostFrequent(wordTable, wordCount, mostFreqWord, &mostFreqCount);
    
    /* 6. 输出结果 */
    printf("统计完成！共发现 %d 个不同的单词\n\n", wordCount);
    
    /* 打印所有单词统计（可选） */
    printAllWords(wordTable, wordCount);
    
    /* 打印最高频单词 */
    printf("-------------------------------------\n");
    printf("【结果】出现次数最多的单词:\n");
    printf("  单词: %s\n", mostFreqWord);
    printf("  次数: %d\n", mostFreqCount);
    printf("=====================================\n");
    
    return 0;
}

void toLowerCase(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + ('a' - 'A');  /* 大写转小写 */
        }
        i++;
    }
}

/* 
 * 判断字符是否为分隔符（非字母字符）
 * 参数: c - 待判断的字符
 * 返回: 1是分隔符，0不是分隔符
 */
int isDelimiter(char c) {
    /* 字母返回0（不是分隔符），非字母返回1（是分隔符） */
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        return 0;
    }
    return 1;
}

/* 
 * 在单词表中查找单词
 * 参数: table - 单词表
 *       n - 当前单词数量
 *       word - 要查找的单词
 * 返回: 找到返回索引，未找到返回-1
 */
int findWord(WordEntry table[], int n, const char *word) {
    int i;
    for (i = 0; i < n; i++) {
        /* 逐个字符比较 */
        if (strcmp(table[i].word, word) == 0) {
            return i;  /* 找到，返回索引 */
        }
    }
    return -1;  /* 未找到 */
}

/* 
 * 添加新单词或更新已有单词的计数
 * 参数: table - 单词表
 *       n - 当前单词数量指针（会修改）
 *       word - 要添加的单词
 */
void addOrUpdateWord(WordEntry table[], int *n, const char *word) {
    int idx;
    int i;
    
    /* 先查找单词是否已存在 */
    idx = findWord(table, *n, word);
    
    if (idx >= 0) {
        /* 单词已存在，计数+1 */
        table[idx].count++;
    } else {
        /* 单词不存在，添加新条目 */
        if (*n >= MAX_WORDS) {
            printf("警告：单词表已满，无法添加 '%s'\n", word);
            return;
        }
        
        /* 复制单词到新条目 */
        i = 0;
        while (word[i] != '\0' && i < MAX_WORD_LEN - 1) {
            table[*n].word[i] = word[i];
            i++;
        }
        table[*n].word[i] = '\0';  /* 字符串结束符 */
        
        table[*n].count = 1;  /* 初始计数为1 */
        (*n)++;  /* 单词总数+1 */
    }
}

/* 
 * 从一行文本中提取并处理单词
 * 参数: table - 单词表
 *       wordCount - 单词数量指针
 *       line - 一行文本
 */
void processLine(WordEntry table[], int *wordCount, char *line) {
    char word[MAX_WORD_LEN];
    int i = 0;      /* line的索引 */
    int j = 0;      /* word的索引 */
    
    while (line[i] != '\0') {
        /* 跳过非字母字符（分隔符） */
        while (line[i] != '\0' && isDelimiter(line[i])) {
            i++;
        }
        
        /* 收集连续的字母字符形成单词 */
        j = 0;
        while (line[i] != '\0' && !isDelimiter(line[i]) && j < MAX_WORD_LEN - 1) {
            word[j] = line[i];
            i++;
            j++;
        }
        
        /* 如果收集到了单词 */
        if (j > 0) {
            word[j] = '\0';  /* 添加字符串结束符 */
            toLowerCase(word);  /* 转为小写统一统计 */
            addOrUpdateWord(table, wordCount, word);
        }
    }
}

/* 
 * 找出出现次数最多的单词
 * 参数: table - 单词表
 *       n - 单词数量
 *       resultWord - 输出：最高频单词
 *       resultCount - 输出：最高频次数
 */
void findMostFrequent(WordEntry table[], int n, char *resultWord, int *resultCount) {
    int maxIdx = 0;
    int i;
    int j;
    
    /* 遍历查找最大计数 */
    for (i = 1; i < n; i++) {
        if (table[i].count > table[maxIdx].count) {
            maxIdx = i;
        }
    }
    
    /* 复制结果 */
    i = 0;
    while (table[maxIdx].word[i] != '\0') {
        resultWord[i] = table[maxIdx].word[i];
        i++;
    }
    resultWord[i] = '\0';
    
    *resultCount = table[maxIdx].count;
}

/* 
 * 打印所有单词统计结果
 * 参数: table - 单词表
 *       n - 单词数量
 */
void printAllWords(WordEntry table[], int n) {
    int i;
    printf("-------------------------------------\n");
    printf("所有单词统计（按发现顺序）：\n");
    printf("%-20s %s\n", "单词", "次数");
    printf("-------------------------------------\n");
    
    for (i = 0; i < n; i++) {
        printf("%-20s %d\n", table[i].word, table[i].count);
    }
    printf("\n");
}
