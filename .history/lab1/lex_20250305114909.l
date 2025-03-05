%{
#include <FlexLexer.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void comment(void);
int line_num = 1; // 用于统计行数
%}

WHITE_SPACE [\ \t\b\f]
DIGIT      [0-9]+
ID         [a-zA-Z_][a-zA-Z0-9_]*
DOT         \.
KEYWORD    (auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|register|restrict|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)

%%
{WHITE_SPACE}   // 忽略空白

"/*"        { comment(); }
"//"[^\n]* { /* 忽略单行注释 */ }
{DIGIT}+([eE][+-]?{DIGIT}+)?          { printf("IntNum: %s\n", yytext); }
{DIGIT}+{DOT}{DIGIT}+([eE][+-]?{DIGIT}+)? { printf("FloatNum: %s\n", yytext); }
{KEYWORD}                            { printf("KEYWORD: %s\n", yytext); }
{ID}                                 { printf("Identifier: %s\n", yytext); }
\"(\\.|[^\\\"])*\"                  { printf("const_string: %s\n", yytext); }
'(\\.|[^\\'])+'                      { printf("const_char: %s\n", yytext); }

"..."      { printf("ELLIPSIS: %s\n", yytext); }
">>="      { printf("RIGHT_ASSIGN: %s\n", yytext); }
"<<="      { printf("LEFT_ASSIGN: %s\n", yytext); }
"+="       { printf("ADD_ASSIGN: %s\n", yytext); }
"-="       { printf("SUB_ASSIGN: %s\n", yytext); }
"*="       { printf("MUL_ASSIGN: %s\n", yytext); }
"/="       { printf("DIV_ASSIGN: %s\n", yytext); }
"%="       { printf("MOD_ASSIGN: %s\n", yytext); }
"&="       { printf("AND_ASSIGN: %s\n", yytext); }
"^="       { printf("XOR_ASSIGN: %s\n", yytext); }
"|="       { printf("OR_ASSIGN: %s\n", yytext); }
">>"       { printf("RIGHT_OP: %s\n", yytext); }
"<<"       { printf("LEFT_OP: %s\n", yytext); }
"++"       { printf("INC_OP: %s\n", yytext); }
"--"       { printf("DEC_OP: %s\n", yytext); }
"->"       { printf("PTR_OP: %s\n", yytext); }
"&&"       { printf("AND_OP: %s\n", yytext); }
"||"       { printf("OR_OP: %s\n", yytext); }
"<="       { printf("LE_OP: %s\n", yytext); }
">="       { printf("GE_OP: %s\n", yytext); }
"=="       { printf("EQ_OP: %s\n", yytext); }
"!="       { printf("NE_OP: %s\n", yytext); }
";"        { printf("';': %s\n", yytext); }
"{"        { printf("'{': %s\n", yytext); }
"}"        { printf("'}': %s\n", yytext); }
","        { printf("',': %s\n", yytext); }
":"        { printf("':': %s\n", yytext); }
"="        { printf("'=': %s\n", yytext); }
"("        { printf("'(': %s\n", yytext); }
")"        { printf("')': %s\n", yytext); }
"["        { printf("'[': %s\n", yytext); }
"]"        { printf("']': %s\n", yytext); }
"."        { printf("'.': %s\n", yytext); }
"&"        { printf("'&': %s\n", yytext); }
"!"        { printf("'!': %s\n", yytext); }
"~"        { printf("'~': %s\n", yytext); }
"-"        { printf("'-': %s\n", yytext); }
"+"        { printf("'+': %s\n", yytext); }
"*"        { printf("'*': %s\n", yytext); }
"/"        { printf("'/': %s\n", yytext); }
"%"        { printf("percent: %s\n", yytext); }
"<"        { printf("'<': %s\n", yytext); }
">"        { printf("'>': %s\n", yytext); }
"^"        { printf("'^': %s\n", yytext); }
"|"        { printf("'|': %s\n", yytext); }
"?"        { printf("'?': %s\n", yytext); }

.          { printf("ERROR: %s\n", yytext); }
\n          { line_num++; } // 统计行数

%%

int main(int argc, char **argv)
{
    if (argc > 1) yyin = fopen(argv[1], "r");
    else printf("error:\n command: lexC filename");
    yylex();
    printf("Total lines: %d\n", line_num); // 输出总行数
    return 0;
}

int yywrap()
{
    return 1;
}

void comment(void)
{
    char c, prev = 0;
    while ((c = input()) != 0)
    {
        if (c == '/' && prev == '*')
            return;
        prev = c;
    }
}