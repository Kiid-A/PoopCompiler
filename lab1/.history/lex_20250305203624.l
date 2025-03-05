%{
#include <FlexLexer.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void jump_comment(void);
int line_count = 1; // 用于统计行数
int char_count = 0; // 用于统计字符总数
int keyword_count = 0; // 用于统计关键字个数
int id_count = 0; // 用于统计标识符个数
int op_count = 0; // 用于统计OP个数
int num_count = 0;
int comment_count = 0;
int comment_error = 0;

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
        printf("ERROR: Unclosed comment at line %d\n", line_count);
        char_count += yyleng;
        comment_error = 0;  // 重置标志
    } else {
        // 记录注释起始位置
        comment_start_pos = ftell(yyin);
        comment_start_line = line_count;
        comment_start_char = char_count;
        jump_comment();  // 进入注释处理
    }
}
"//"[^\n]*      { char_count += yyleng; } // 统计单行注释
{DIGIT}+        { PRINT_TOKEN("IntNum"); num_count++; }
{DIGIT}+{DOT}{DIGIT}*   { PRINT_TOKEN("FloatNum"); num_count++; }
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

.          { printf("ERROR: %s in line: %d\n", yytext, line_count); char_count++; }
\n         { line_count++; char_count++; } // 统计行数

%%

void print_info() {
    printf("Total lines: %d\n", line_count); // 输出总行数
    printf("Total characters: %d\n", char_count); // 输出字符总数
    printf("Total keywords: %d\n", keyword_count); // 输出关键字个数
    printf("Total identifiers: %d\n", id_count); // 输出标识符个数
    printf("Total ops: %d\n", op_count); // 输出OP个数
    printf("Total nums: %d\n", num_count);
    printf("Total comments: %d\n", comment_count);
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
    int beggining = line_count;
    char c, prev = 0;
    while ((c = yyinput()) != 0)
    {
        if (c == '/' && prev == '*') {
            comment_count += beggining - line_count;
            return;
        }
        prev = c;
        if (c == '\n')
    }
    printf("ERROR: Unterminated comment in line: %d\n", beggining);
    // 将输入流移回到注释开始的位置
    yyless(0); // 将当前 token 的输入流回退到注释开始的位置
    line_count = beggining; // 恢复行号
    return;
}