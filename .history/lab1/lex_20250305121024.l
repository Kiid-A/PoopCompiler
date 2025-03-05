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
%}

WHITE_SPACE [\ \t\b\f]
DIGIT      [0-9]+
ID         [a-zA-Z_][a-zA-Z0-9_]*
DOT         \.
KEYWORD    (auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|register|restrict|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)

%%
{WHITE_SPACE}   {char_count += yyleng;}
"/*" { jump_comment(); }
"//"[^\n]* { char_count += yyleng; }
{DIGIT}+([eE][+-]?{DIGIT}+)? {
    printf("IntNum: %s\n", yytext); 
    char_count += yyleng; 
}
{DIGIT}+{DOT}{DIGIT}+([eE][+-]?{DIGIT}+)? { 
    printf("FloatNum: %s\n", yytext); 
    char_count += yyleng; 
}
{KEYWORD} {
    printf("KEYWORD: %s\n", yytext); 
    char_count += yyleng; 
    keyword_count++;
}
{ID} { 
    printf("Identifier: %s\n", yytext); 
    char_count += yyleng; 
}
\"(\\.|[^\\\"])*\" { 
    printf("const_string: %s\n", yytext); 
    char_count += yyleng; 
}
'(\\.|[^\\'])+' { 
    printf("const_char: %s\n", yytext); 
    char_count += yyleng; 
}

"..."      { printf("ELLIPSIS: %s\n", yytext); }
">>="      { printf("RSHIFT_ASSIGN: %s\n", yytext); }
"<<="      { printf("LSHIFT_ASSIGN: %s\n", yytext); }
"+="       { printf("ADD_ASSIGN: %s\n", yytext); }
"-="       { printf("SUB_ASSIGN: %s\n", yytext); }
"*="       { printf("MUL_ASSIGN: %s\n", yytext); }
"/="       { printf("DIV_ASSIGN: %s\n", yytext); }
"%="       { printf("MOD_ASSIGN: %s\n", yytext); }
"&="       { printf("AND_ASSIGN: %s\n", yytext); }
"^="       { printf("XOR_ASSIGN: %s\n", yytext); }
"|="       { printf("OR_ASSIGN: %s\n", yytext); }
">>"       { printf("RSHIFT_OP: %s\n", yytext); }
"<<"       { printf("LSHIFT_OP: %s\n", yytext); }
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
"%"        { printf("'%': %s\n", yytext); }
"<"        { printf("'<': %s\n", yytext); }
">"        { printf("'>': %s\n", yytext); }
"^"        { printf("'^': %s\n", yytext); }
"|"        { printf("'|': %s\n", yytext); }
"?"        { printf("'?': %s\n", yytext); }

.          { printf("ERROR: %s\n", yytext); char_count++; }
\n         { line_num++; char_count++; } // 统计行数

%%

void print_info() {
    printf("Total lines: %d\n, ", line_num);
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