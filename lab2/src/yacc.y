// %{
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "lex.h"


// void yyerror(char *s) {
//     fprintf(stderr, "Error: %s\n", s);
// }

// int current_position = 0;
// char *input_string = "3+2-2*5";

// void print_analysis_status() {
//     if (input_string == NULL) {
//         fprintf(stderr, "Error: input_string is not initialized.\n");
//         return;
//     }
//     printf("Stack: ");
//     for (int i = 0; i < current_position; i++) {
//         printf("%c", input_string[i]);
//     }
//     printf(" | Input: ");
//     for (int i = current_position; input_string[i] != '\0'; i++) {
//         printf("%c", input_string[i]);
//     }
//     printf("\n");
// }
// %}

// %token NUMBER
// %left PLUS MINUS
// %left MULT DIV
// %token LPAREN RPAREN

// %%

// start: E {
//         printf("Done!\n");
//     }
//     ;

// E: E PLUS T {
//         printf("E → E+T\n");
//         print_analysis_status();
//     }
//     | E MINUS T {
//         printf("E → E-T\n");
//         print_analysis_status();
//     }
//     | T {
//         printf("E → T\n");
//         print_analysis_status();
//     }
//     ;

// T: T MULT F {
//         printf("T → T*F\n");
//         print_analysis_status();
//     }
//     | T DIV F {
//         printf("T → T/F\n");
//         print_analysis_status();
//     }
//     | F {
//         printf("T → F\n");
//         print_analysis_status();
//     }
//     ;

// F: LPAREN E RPAREN {
//         printf("F → (E)\n");
//         print_analysis_status();
//     }
//     | NUMBER {
//         if (yytext != NULL) {
//             printf("F → num\n");
//             print_analysis_status();
//             current_position += strlen(yytext);
//         } else {
//             fprintf(stderr, "Error: yytext is NULL.\n");
//         }
//     }
//     ;

// %%

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"

void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int current_position = 0;
char *input_string = "3+2-2*5\0";

char stack_E[100] = "";
char stack_T[100] = "";
char stack_F[100] = "";

void update_stack_string(char *stack, const char *str) {
    if (strlen(stack) == 0) {
        strcpy(stack, str);
    } else {
        strcat(stack, str);
    }
}

void print_analysis_status() {
    if (input_string == NULL) {
        fprintf(stderr, "Error: input_string is not initialized.\n");
        return;
    }
    printf("Stack: ");
    for (int i = 0; i < current_position; i++) {
        printf("%c", input_string[i]);
    }
    printf(" | Input: ");
    for (int i = current_position; input_string[i] != '\0'; i++) {
        printf("%c", input_string[i]);
    }
    printf("\n");
}
%}

%token NUMBER
%left PLUS MINUS
%left MULT DIV
%token LPAREN RPAREN

%%

start: E {
        printf("Done!\n");
    }
    ;

E: E PLUS T {
        char temp[200];
        sprintf(temp, "%s+%s", stack_E, stack_T);
        strcpy(stack_E, temp);
        printf("E → E+T (%s)\n", temp);
        current_position += strlen(stack_T) + 1;
        print_analysis_status();
    }
    | E MINUS T {
        char temp[200];
        sprintf(temp, "%s-%s", stack_E, stack_T);
        strcpy(stack_E, temp);
        printf("E → E-T (%s)\n", temp);
        current_position += strlen(stack_T) + 1;
        print_analysis_status();
    }
    | T {
        strcpy(stack_E, stack_T);
        printf("E → T (%s)\n", stack_T);
        current_position += strlen(stack_T);
        print_analysis_status();
    }
    ;

T: T MULT F {
        char temp[200];
        sprintf(temp, "%s*%s", stack_T, stack_F);
        strcpy(stack_T, temp);
        printf("T → T*F (%s)\n", temp);
        current_position += strlen(stack_F) + 1;
        print_analysis_status();
    }
    | T DIV F {
        char temp[200];
        sprintf(temp, "%s/%s", stack_T, stack_F);
        strcpy(stack_T, temp);
        printf("T → T/F (%s)\n", temp);
        current_position += strlen(stack_F) + 1;
        print_analysis_status();
    }
    | F {
        strcpy(stack_T, stack_F);
        printf("T → F (%s)\n", stack_F);
        current_position += strlen(stack_F);
        print_analysis_status();
    }
    ;

F: LPAREN E RPAREN {
        char temp[200];
        sprintf(temp, "(%s)", stack_E);
        strcpy(stack_F, temp);
        printf("F → (E) (%s)\n", temp);
        current_position += strlen(stack_E) + 2;
        print_analysis_status();
    }
    | NUMBER {
        if (yytext != NULL) {
            strcpy(stack_F, yytext);
            printf("F → num (%s)\n", yytext);
            current_position += strlen(yytext);
            print_analysis_status();
        } else {
            fprintf(stderr, "Error: yytext is NULL.\n");
        }
    }
    ;

%%