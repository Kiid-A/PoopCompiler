#include <string>
#include <vector>
#include <regex>

// a simple state machine
// parse text sep by white space
class Lexer {
    enum TokenType {
        TOKEN_CONSTSTR,
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

    enum State {
        STOP,
        RUNNING,
        PARSE_INCLUDE, // #include<>""
        PARSE_CONSTSTR, // const words
        PARSE_WORD, // keyword, identifier, 
        PARSE_NUM,  // numbers
        PARSE_OP,   // operators, assignments(ended with '=')
        PARSE_COMMENT   // one line, multiple lines
    };

    typedef struct Token {
        TokenType type_;
        std::string value_;
        int line_;

        Token(TokenType t, std::string v, int l) 
            : type_(t), value_(v), line_(l) {}; 
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
        "||", "!", "&", "|", "^", "~", "<<", ">>", ".", "->", "[", "]", "{", "}",
        ":", "?"
    };
    const std::vector<std::string> assignments = {
        "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|="
    };
    const std::vector<char> whitespace = {
        ' ', "\t", "\b", "\f"
    };

    int lines_ = 1; // 用于统计行数
    int chars_ = 0; // 用于统计字符总数
    int keywords_ = 0; // 用于统计关键字个数
    int ids_ = 0; // 用于统计标识符个数
    int ops_ = 0; // 用于统计OP个数
    int comments_ = 0;  // 用于统计注释个数

    int state_ = STOP;
    int cur_ = 0;   // current ptr
    std::vector<char> buffer_;  // current buffer
    std::vector<Token> tokens_;

    // checker
    bool isOp(const std::string& s) {
        for (auto op : operators) {
            if (op == s) {
                return true;
            }
        }
        return false;
    }

    bool isOpChar(char c) {
        static const std::string ops = "+-*/%=&|^~<>!.";
        return ops.find(c) != std::string::npos;
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

    bool isWhiteSpace(const std::string& s) {
        for (auto ws : whitespace) {
            if (s == ws) {
                return true;
            }
        }
        return false;
    }

    bool isDigit(char ch) {
        return ch >= '0' && ch <= '9';
    }

    // remember to check '_' if needed
    bool isLetter(char ch) {
        return ch >= 'a' && ch <= 'z' || 
               ch >= 'A' && ch <= 'Z';
    }

    bool isId(char ch) {
        return isLetter(ch) || isDigit(ch);        
    }

    // advance one step
    // check word, check digit, check op and assignment
    void advance();

    void skipWhiteSpace();

    void skipComment();

    // parse until next token
    Token nextToken();

    void analyze();

    void parseComment(bool isBlock);
    // when parse an unrecognised word, throw error
    void parseError();
    void parseInclude();
    void parseNumber();
    void parseConstStr(char quote);
    void parseOperator();
    void parseWord();

public:
    Lexer() {}
    ~Lexer() {}

    // @brief Read words into buffer, sep words by whitespace
    void ReadFile(const std::string& filename);

    void StartAnalyse();

    // @brief Print what the hell this poop lexer has analysed
    void PrintInfo() const;

    const std::vector<Token>& GetTokens() const { return tokens_; }
};
