#include <iostream>
#include <vector>

class Lexer {
    enum TokenType {
        TOKEN_KEYWORD,
        TOKEN_IDENTIFIER,
        TOKEN_NUMBER,
        TOKEN_OPERATOR,
        TOKEN_PUNCTUATION,
        TOKEN_EOF,
        TOKEN_ERROR
    };

    typedef struct {
        TokenType type_;
        std::string value_;
    } Token;

private:
    const std::vector<std::string> keywords = {
        "auto", "break", "case", "char", "const", "continue",
        "default", "do", "double", "else", "enum", "extern",
        "float", "for", "goto", "if", "int", "long",
        "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void",
        "volatile", "while"
    };

    int lines_ = 1; // 用于统计行数
    int chars_ = 0; // 用于统计字符总数
    int keywords_ = 0; // 用于统计关键字个数
    int ids_ = 0; // 用于统计标识符个数
    int ops_ = 0; // 用于统计OP个数
    int comments_ = 0;  // 用于统计注释个数

    // checker
    bool isOp(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || 
               c == '=' || c == '<' || c == '>';
    }

    bool isKeyword(const std::string& s) {
        for (auto kw : ke)
    }

public:
    Lexer() {}
    ~Lexer() {}
};

int main(int argc, char **argv)
{
    if (argc > 1) = fopen(argv[1], "r");
    else printf("error:\n command: lexC filename");
    return 0;
}