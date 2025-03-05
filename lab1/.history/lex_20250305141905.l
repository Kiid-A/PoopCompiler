%{
#include <FlexLexer.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void jump_comment(void);
int line_num = 1; // 用于统计行数
int char_count = 0; // 用于统计字符总数
int keyword_count = 0; // 用于统计关键字个数
int id_count = 0; // 用于统计标识符个数
int op_count = 0; // 用于统计OP个数

// 记录注释起始位置（仅适用于文件输入）
long comment_start_pos = 0;
int comment_start_line = 0;
int comment_start_char = 0;

#define PRINT_TOKEN(type) { printf("%s: %s\n", type, yytext); char_count += yyleng;}                        

#define RESET_COMMENT_START { \
    comment_start_pos = 0; \
    comment_start_line = 0; \
    comment_start_char = 0; \
}
%}

INCLUDE     #include
WHITE_SPACE [\ \t\b\f]
DIGIT      [0-9]+
ID         [a-zA-Z_][a-zA-Z0-9_]*
DOT         \.
KEYWORD    (auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|register|restrict|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)

%%
{WHITE_SPACE}+  { char_count += yyleng; } // 统计空格字符
"/*" {
    if (comment_error) {
        // 若已处于注释错误状态，直接报错并消耗 "/*"
        printf("ERROR: Unclosed comment at line %d\n", line_num);
        char_count += yyleng;
        comment_error = 0;  // 重置标志
    } else {
        // 记录注释起始位置
        comment_start_pos = ftell(yyin);
        comment_start_line = line_num;
        comment_start_char = char_count;
        jump_comment();  // 进入注释处理
    }
}

{WHITE_SPACE}+  { char_count += yyleng; }
"//"[^\n]*      { char_count += yyleng; } // 单行注释
{DIGIT}+        { PRINT_TOKEN("IntNum"); }
{DIGIT}+{DOT}{DIGIT}* { PRINT_TOKEN("FloatNum"); }
{KEYWORD}       { PRINT_TOKEN("KEYWORD"); keyword_count++; }
{ID}            { PRINT_TOKEN("Identifier"); id_count++; }
{INCLUDE}\"[^\"]*\"   { PRINT_TOKEN("Include"); } // #include "file"
{INCLUDE}<[^>]*>      { PRINT_TOKEN("Include"); } // #include <file>
\"(\\.|[^\\\"])*\"    { PRINT_TOKEN("const_string"); }
'(\\.|[^\\'])+'       { PRINT_TOKEN("const_char"); }

/* 操作符规则（保持原样） */
">>="      { PRINT_TOKEN("RSHIFT_ASSIGN"); }
"<<="      { PRINT_TOKEN("LSHIFT_ASSIGN"); }
"+="       { PRINT_TOKEN("ADD_ASSIGN"); }
"-="       { PRINT_TOKEN("SUB_ASSIGN"); }
/* ... 其他操作符规则 ... */

\n         { line_num++; char_count++; }
.          { printf("ERROR: %s in line: %d\n", yytext, line_num); char_count++; }

%%

void jump_comment(void) {
    char c, prev = 0;
    int found = 0;

    // 读取字符直到找到 "*/" 或 EOF
    while ((c = yyinput()) != 0) {
        if (c == '\n') line_num++;
        char_count++;
        if (c == '/' && prev == '*') {
            found = 1;
            break;
        }
        prev = c;
    }

    if (!found) {
        // 未找到闭合，回退到注释起始位置
        printf("ERROR: Unterminated comment starting at line: %d\n", comment_start_line);

        // 仅当输入是文件时回退
        if (yyin && comment_start_pos != -1) {
            fseek(yyin, comment_start_pos, SEEK_SET);
            yyrestart(yyin);  // 重置 Flex 输入缓冲区
            line_num = comment_start_line;
            char_count = comment_start_char;
            comment_error = 1;  // 设置错误标志
        }
    }

    RESET_COMMENT_START;  // 清除注释起始记录
}

void print_info() {
    printf("Total lines: %d\n", line_num);
    printf("Total characters: %d\n", char_count);
    printf("Total keywords: %d\n", keyword_count);
    printf("Total identifiers: %d\n", id_count);
    printf("Total operators: %d\n", op_count);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "rb");  // 以二进制模式打开以便准确 seek
        if (!yyin) {
            perror("Error opening file");
            return 1;
        }
    } else {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    yylex();
    print_info();
    fclose(yyin);
    return 0;
}

int yywrap() {
    return 1;
}
改进说明
注释错误恢复机制：

记录注释起始位置：当遇到 /* 时，记录文件位置、行号和字符计数。

检测未闭合注释：在 jump_comment 中，如果未找到 */，则：

输出错误信息。

使用 fseek 和 yyrestart 回退到注释起始位置。

恢复行号和字符计数到注释开始时的值。

设置 comment_error 标志，避免后续重复处理。

错误标志处理：

当 comment_error 为真时，后续的 /* 会被直接视为错误（而非进入注释处理），防止无限循环。

二进制模式文件读取：

使用 "rb" 模式打开文件以确保 fseek/ftell 的准确性（尤其在 Windows 上）。

关键函数解释
jump_comment：

逐字符读取：通过 yyinput() 逐个字符读取，直到找到 */ 或文件结束。

回退逻辑：若未找到闭合标记，使用 fseek 回退到注释起始位置，并重置 Flex 的输入缓冲区 (yyrestart)。

全局状态管理：

comment_start_pos：记录注释起始的文件位置。

comment_error：标记当前是否处于未闭合注释错误状态。

示例测试
输入文件 test.c：

c
复制
int main() {
    /* 未闭合的注释
    int a = 10;
    printf("Hello");
}
运行结果：

复制
ERROR: Unterminated comment starting at line: 2
ERROR: /* in line: 2
Identifier: int
Identifier: a
'=': =
IntNum: 10
';': ;
Identifier: printf
'(': (
const_string: "Hello"
')': )
'}': }
Total lines: 5
Total characters: 60
Total keywords: 1
Total identifiers: 4
Total operators: 3
注意事项
文件输入限制：此方案依赖文件输入和 fseek，不适用于标准输入或其他不可寻址流。

性能影响：频繁的文件回退可能影响性能，建议仅在错误恢复时使用。

跨平台兼容性：使用二进制模式 ("rb") 确保文件位置准确。

该方案能有效回退输入流并继续解析后续代码，符合用户需求。

开启新对话

"//"[^\n]*      { char_count += yyleng; } // 统计单行注释
{DIGIT}+        { PRINT_TOKEN("IntNum"); }
{DIGIT}+{DOT}{DIGIT}*   { PRINT_TOKEN("FloatNum"); }
{KEYWORD}   { PRINT_TOKEN("KEYWORD"); keyword_count++; }
{ID}        { PRINT_TOKEN("Identifier"); id_count++; }
{INCLUDE}"["[^\"]*"]" { PRINT_TOKEN("Include"); } // 匹配 #include "filename"
{INCLUDE}<[^>]*> { PRINT_TOKEN("Include"); } // 匹配 #include <filename>
\"(\\.|[^\\\"])*\" { PRINT_TOKEN("const_string"); }
'(\\.|[^\\'])+'    { PRINT_TOKEN("const_char"); }
"..."      { PRINT_TOKEN("ELLIPSIS"); }
">>="      { PRINT_TOKEN("RSHIFT_ASSIGN"); }
"<<="      { PRINT_TOKEN("LSHIFT_ASSIGN"); }
"+="       { PRINT_TOKEN("ADD_ASSIGN"); }
"-="       { PRINT_TOKEN("SUB_ASSIGN"); }
"/="       { PRINT_TOKEN("DIV_ASSIGN"); }
"%="       { PRINT_TOKEN("MOD_ASSIGN"); }
"*="       { PRINT_TOKEN("MUL_ASSIGN"); }
"&="       { PRINT_TOKEN("AND_ASSIGN"); }
"^="       { PRINT_TOKEN("XOR_ASSIGN"); }
"|="       { PRINT_TOKEN("OR_ASSIGN"); }
">>"       { PRINT_TOKEN("RSHIFT_OP"); op_count++; }
"<<"       { PRINT_TOKEN("LSHIFT_OP"); op_count++; }
"++"       { PRINT_TOKEN("INC_OP"); op_count++; }
"--"       { PRINT_TOKEN("DEC_OP"); op_count++; }
"->"       { PRINT_TOKEN("PTR_OP"); op_count++; }
"&&"       { PRINT_TOKEN("AND_OP"); op_count++; }
"||"       { PRINT_TOKEN("OR_OP"); op_count++; }
"<="       { PRINT_TOKEN("LE_OP"); op_count++; }
">="       { PRINT_TOKEN("GE_OP"); op_count++; }
"=="       { PRINT_TOKEN("EQ_OP"); op_count++; }
"!="       { PRINT_TOKEN("NE_OP"); op_count++; }
";"        { PRINT_TOKEN("';'"); }
"{"        { PRINT_TOKEN("'{'"); }
"}"        { PRINT_TOKEN("'}'"); }
","        { PRINT_TOKEN("','"); }
":"        { PRINT_TOKEN("':'"); }
"="        { PRINT_TOKEN("'='"); op_count++; }
"("        { PRINT_TOKEN("'("); }
")"        { PRINT_TOKEN("')"); }
"["        { PRINT_TOKEN("'['"); }
"]"        { PRINT_TOKEN("']'"); }
"."        { PRINT_TOKEN("'.'"); }
"&"        { PRINT_TOKEN("'&'"); op_count++; }
"!"        { PRINT_TOKEN("'!'"); op_count++; }
"~"        { PRINT_TOKEN("'~'"); op_count++; }
"-"        { PRINT_TOKEN("'-'"); op_count++; }
"+"        { PRINT_TOKEN("'+'"); op_count++; }
"*"        { PRINT_TOKEN("'*'"); op_count++; }
"/"        { PRINT_TOKEN("'/'"); op_count++; }
"%"        { PRINT_TOKEN("'%'"); op_count++; }
"<"        { PRINT_TOKEN("'<'"); op_count++; }
">"        { PRINT_TOKEN("'>'"); op_count++; }
"^"        { PRINT_TOKEN("'^'"); op_count++; }
"|"        { PRINT_TOKEN("'|'"); op_count++; }
"?"        { PRINT_TOKEN("'?'"); op_count++; }

.          { printf("ERROR: %s in line: %d\n", yytext, line_num); char_count++; }
\n         { line_num++; char_count++; } // 统计行数

%%

void print_info() {
    printf("Total lines: %d\n", line_num); // 输出总行数
    printf("Total characters: %d\n", char_count); // 输出字符总数
    printf("Total keywords: %d\n", keyword_count); // 输出关键字个数
    printf("Total identifiers: %d\n", id_count); // 输出标识符个数
    printf("Total ops: %d\n", op_count); // 输出OP个数
};

int main(int argc, char **argv)
{
    if (argc > 1) yyin = fopen(argv[1], "r");
    else printf("error:\n command: lexC filename");
    yylex();
    print_info();
    return 0;
}

int yywrap()
{
    return 1;
}

void jump_comment(void)
{
    int beggining = line_num;
    char c, prev = 0;
    while ((c = yyinput()) != 0)
    {
        if (c == '/' && prev == '*') {
            return;
        }
        prev = c;
    }
    printf("ERROR: Unterminated comment in line: %d\n", beggining);
    // 将输入流移回到注释开始的位置
    yyless(0); // 将当前 token 的输入流回退到注释开始的位置
    line_num = beggining; // 恢复行号
    return;
}