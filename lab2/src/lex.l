%{
#include "yacc.tab.h"
%}
%%
[0-9]+ { yylval = atoi(yytext); return NUMBER; }
[+]    { return PLUS; }
[-]    { return MINUS; }
[*]    { return MULT; }
[/]    { return DIV; }
[(]    { return LPAREN; }
[)]    { return RPAREN; }
[ \t\n] /* empty */ 
.      { perror("非法字符"); }
%%
int yywrap() { return 1; }