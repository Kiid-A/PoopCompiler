#include <iostream>

int main(int argc, char **argv)
{
    if (argc > 1) yyin = fopen(argv[1], "r");
    else printf("error:\n command: lexC filename");
    yylex();
    print_info();
    return 0;
}