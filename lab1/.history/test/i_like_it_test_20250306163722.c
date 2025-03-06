
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 全局变量
int global_var = 10;

// 函数声明
void function_declaration(int param);

// 结构体定义
struct Point {
    int x;
    int y;
};

// 联合体定义
union Data {
    int i;
    float f;
    char str[20];
};

// 枚举定义
enum Color { RED, GREEN, BLUE };

// 宏定义
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// 函数定义
void function_declaration(int param) {
    // 局部变量
    int local_var = param + 10;
    printf("Local variable: %d\n", local_var);
}

// 主函数
int main() {
    // 变量声明
    int a = 5, b = 10;
    float f = 3.14;
    char c = 'A';
    char str[] = "Hello, World!";
    int *ptr = &a;

    // 运算符
    int sum = a + b;
    int diff = a - b;
    int product = a * b;
    int quotient = a / b;
    int remainder = a % b;

    // 递增和递减运算符
    a++;
    b--;

    // 关系运算符
    int is_greater = (a > b) ? 1 : 0;
    int is_less = (a < b) ? 1 : 0;
    int is_equal = (a == b) ? 1 : 0;

    // 逻辑运算符
    int logical_and = (a > 0 && b > 0) ? 1 : 0;
    int logical_or = (a > 0 || b < 0) ? 1 : 0;
    int logical_not = (!a) ? 1 : 0;

    // 位运算符
    int bitwise_and = a & b;
    int bitwise_or = a | b;
    int bitwise_xor = a ^ b;
    int bitwise_not = ~a;
    int left_shift = a << 2;
    int right_shift = a >> 2;

    // 条件运算符
    int max = MAX(a, b);

    // 循环
    for (int i = 0; i < 5; i++) {
        printf("Loop iteration: %d\n", i);
    }

    // 条件语句
    if (a > b) {
        printf("a is greater than b\n");
    } else if (a < b) {
        printf("a is less than b\n");
    } else {
        printf("a is equal to b\n");
    }

    // switch语句
    switch (a) {
        case 5:
            printf("a is 5\n");
            break;
        case 10:
            printf("a is 10\n");
            break;
        default:
            printf("a is not 5 or 10\n");
    }

    // 函数调用
    function_declaration(a);

    // 结构体使用
    struct Point p1 = {1, 2};
    printf("Point p1: (%d, %d)\n", p1.x, p1.y);

    // 联合体使用
    union Data data;
    data.i = 10;
    printf("Union data.i: %d\n", data.i);

    // 枚举使用
    enum Color color = RED;
    printf("Color: %d\n", color);

    // 动态内存分配
    int *dynamic_array = (int *)malloc(5 * sizeof(int));
    if (dynamic_array != NULL) {
        for (int i = 0; i < 5; i++) {
            dynamic_array[i] = i * 10;
        }
        free(dynamic_array);
    }

    // 文件操作
    FILE *file = fopen("test.txt", "w");
    if (file != NULL) {
        fprintf(file, "Writing to file\n");
        fclose(file);
    }

    // 字符串操作
    char str1[20] = "Hello";
    char str2[20] = "World";
    strcat(str1, " ");
    strcat(str1, str2);
    printf("Concatenated string: %s\n", str1);

    // 数学函数
    double sqrt_result = sqrt(a);
    printf("Square root of %d: %f\n", a, sqrt_result);

    return 0;
}