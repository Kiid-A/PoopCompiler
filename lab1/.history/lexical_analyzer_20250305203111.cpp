#include <iostream>

class LexAnalyzer {
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
    int line_count = 1; // 用于统计行数
int char_count = 0; // 用于统计字符总数
int keyword_count = 0; // 用于统计关键字个数
int id_count = 0; // 用于统计标识符个数
int op_count = 0; // 用于统计OP个数
int comment_error = 0;
public:
    LexAnalyzer() {}
    ~LexAnalyzer() {}
};

int main(int argc, char **argv)
{
    if (argc > 1)  = fopen(argv[1], "r");
    else printf("error:\n command: lexC filename");
    return 0;
}