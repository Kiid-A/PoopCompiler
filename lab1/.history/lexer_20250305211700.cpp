#include <iostream>
#include <vector>

class Lexer {
    enum TokenType {
        TOKEN_KEYWORD,
        TOKEN_IDENTIFIER,
        TOKEN_NUMBER,
        TOKEN_OPERATOR,
        TOKEN_ASSIGNMENT,
        TOKEN_PUNCTUATION,
        TOKEN_COMMENT,
        TOKEN_EOF,
        TOKEN_ERROR
    };

    typedef struct Token {
        TokenType type_;
        std::string value_;

        Token(TokenType t, std::string v) : type_(t), value_(v) {}; 
    } Token;

private:
    const std::vector<std::string> keyword = {
        "auto", "break", "case", "char", "const", "continue",
        "default", "do", "double", "else", "enum", "extern",
        "float", "for", "goto", "if", "int", "long",
        "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void",
        "volatile", "while"
    };
    const std::vector<std::string> operators = {
        "+", "-", "*", "/", "%", "==", "!=", "<", ">", "<=", ">=", "&&", 
        "||", "!", "&", "|", "^", "~", "<<", ">>"
    };
    const std::vector<std::string> assignments = {
        "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|="
    };

    int lines_ = 1; // 用于统计行数
    int chars_ = 0; // 用于统计字符总数
    int keywords_ = 0; // 用于统计关键字个数
    int ids_ = 0; // 用于统计标识符个数
    int ops_ = 0; // 用于统计OP个数
    int comments_ = 0;  // 用于统计注释个数

    // checker
    bool isOp(const std::string& s) {
        for (auto op : operators) {
            if (op == s) {
                return true;
            }
        }
        return false;
    }

    bool isKeyword(const std::string& s) {
        for (auto kw : keyword) {
            if (kw == s) {
                return true;
            }
        }
        return false;
    }

    bool isAssign(const std::string& s) {
        for (auto as : assignments) {
            if (as == s) {
                return true;
            }
        }
        return false;
    }

    bool isWhiteSpace()




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