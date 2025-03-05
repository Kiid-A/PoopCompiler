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
    int line

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