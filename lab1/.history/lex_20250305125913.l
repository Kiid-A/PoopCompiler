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

#define PRINT_TOKEN(type) { printf("%s: %s\n", type, yytext); char_count += yyleng;}                        
%}

INCLUDE include
WHITE_SPACE [\ \t\b\f]
DIGIT      [0-9]+
ID         [a-zA-Z_][a-zA-Z0-9_]*
DOT         \.
KEYWORD    (auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|register|restrict|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)

%%
{WHITE_SPACE}+  { char_count += yyleng; } // 统计空格字符
"/*"            { jump_comment(); }
"//"[^\n]*      { char_count += yyleng; } // 统计单行注释
{DIGIT}+        { PRINT_TOKEN("IntNum"); }
{DIGIT}+{DOT}{DIGIT}*   { PRINT_TOKEN("FloatNum"); }
{KEYWORD}   { PRINT_TOKEN("KEYWORD"); keyword_count++; }
{ID}        { PRINT_TOKEN("Identifier"); id_count++; }
{INCLUDE}   { PRINT_TOKEN("include"); }
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
    char c, prev = 0;
    while ((c = yyinput()) != 0)
    {
        if (c == '/' && prev == '*')
            return;
        prev = c;
    }
    printf("ERROR: Unterminated comment\n");
}