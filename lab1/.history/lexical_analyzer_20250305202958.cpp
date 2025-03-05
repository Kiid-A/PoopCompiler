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

    } Token;

private:

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