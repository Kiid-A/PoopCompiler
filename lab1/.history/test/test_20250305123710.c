/* 这是一个测试文件，用于验证词法分析器的错误处理能力 */

// 正确的单行注释

/* 错误的多行注释，没有结束符 

int main() /* 正确的函数定义 */
{
    printf("Hello, World!\n"); // 正确的函数调用

    auto int = 10; // 错误的关键字使用：'auto'不能这样用
    const char* str = "Hello" "World"; // 错误的字符串连接

    for (int i = 0; i < 10; i++) { // 正确的for循环
        printf("%d\n", i);
    }

    +++ // 错误的增量运算符使用位置

    return 0;
} /* 函数定义结束 */

/* 错误的多行注释
   没有开始符 */

void anotherFunction()
{
    prntf("This is an error"); // 错误的函数名
}

int someVariable = 20 // 缺少分号

enum { // 缺少枚举值定义
    RED, GREEN, BLUE
} color;

int main2() // 重复定义的main函数
{
    color = RED;
    printf("Color is %d\n", color);
    return 0;
}