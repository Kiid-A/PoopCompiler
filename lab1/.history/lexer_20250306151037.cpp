#include <iostream>
#include <fstream>
#include "lexer.h"
#include <iomanip>

int step = 0;
#define debug printf("step: %d\n", step++);

void Lexer::parseComment(bool isBlock) {
    std::string comment;
    int start_line = lines_;
    char prev = 0;

    while (buffer_[cur_] != EOF) {
        char c = buffer_[cur_++];
        chars_++;

        if (c == '\n') lines_++;

        if (isBlock) {
            if (prev == '*' && c == '/') {
                comments_++;
                tokens_.emplace_back(TOKEN_COMMENT, comment, start_line);
                return;
            }
            prev = c;
        } else if (c == '\n') {
            comments_++;
            tokens_.emplace_back(TOKEN_COMMENT, comment, start_line);
            return;
        }

        comment += c;
    }

    if (isBlock) {
        tokens_.emplace_back(TOKEN_ERROR, "Unclosed block comment", start_line);
    }
}

void Lexer::parseDefine() {
    std::string defined;
    int start_line = lines_;
    char prev = 0;
    bool nextLine = false;

    while (buffer_[cur_] != EOF) {
        char c = buffer_[cur_++];
        chars_++;

        if (c == '\\') nextLine = true;
        if (c == '\n') { 
            lines_++;
            if (nextLine) nextLine = false;
            else {
                tokens_.emplace_back(TOKEN_DEFINE, defined, start_line);
                return;
            }
        }
        defined += c;
    }

    tokens_.emplace_back(TOKEN_ERROR, "Unclosed define", start_line);
}

void Lexer::parseInclude() {
    std::string path;
    char quote = 0;
    int start_line = lines_;

    while (buffer_[cur_] != EOF) {
        // printf("char: %c\n", buffer_[cur_]);
        char c = buffer_[cur_];
        if (c == '<' || c == '"') {
            quote = c;
            cur_++;
            break;
        }
        cur_++;
    }

    while (buffer_[cur_] != EOF) {
        char c = buffer_[cur_++];
        if (c == '\n') lines_++;
        
        if ((quote == '"' && c == '"') || 
            (quote == '<' && c == '>')) {
            tokens_.emplace_back(TOKEN_INCLUDE, path, start_line);
            return;
        }
        path += c;
    }

    // 未闭合处理
    tokens_.emplace_back(TOKEN_ERROR, "Unclosed include", start_line);
}

void Lexer::parseNumber() {
    std::string num;
    bool hasDot = false;
    int start_line = lines_;

    while (buffer_[cur_] != EOF) {
        char c = buffer_[cur_];
        
        if (isdigit(c)) {
            num += c;
            cur_++;
            chars_++;
        } else if (c == '.' && !hasDot) {
            hasDot = true;
            num += c;
            cur_++;
            chars_++;
        } else {
            break;
        }
    }
    
    tokens_.emplace_back(TOKEN_NUMBER, num, start_line);
}

void Lexer::parseWord() {
    std::string word;
    int start_line = lines_;

    while (buffer_[cur_] != EOF) {
        char c = buffer_[cur_];
        if (!isId(c) && c != '_') break;

        word += c;
        cur_++;
        chars_++;
    }

    if (isKeyword(word)) {
        tokens_.emplace_back(TOKEN_KEYWORD, word, start_line);
        keywords_++;
    } else {
        tokens_.emplace_back(TOKEN_IDENTIFIER, word, start_line);
        ids_++;
    }
}

void Lexer::parseConstStr(char quote) {
   std::string value;
    bool escape = false;
    int start_line = lines_;

    while (buffer_[cur_] != EOF) {
        char c = buffer_[cur_++];
        chars_++;

        if (c == '\n') lines_++;

        if (escape) {
            escape = false;
            value += c;
            continue;
        }

        if (c == '\\') {
            escape = true;
            continue;
        }

        if (c == quote) {
            tokens_.emplace_back(TOKEN_CONSTSTR, value, start_line);
            return;
        }

        value += c;
    }

    tokens_.emplace_back(TOKEN_ERROR, "Unclosed string", start_line);
}

void Lexer::parseOperator() {
    std::string op(1, buffer_[cur_++]);
    chars_++;
    
    for (int lookahead = 0; lookahead < 2; lookahead++) {
        if (cur_ + lookahead >= buffer_.size()) break;
        
        std::string test = op + buffer_[cur_ + lookahead];
        if (isOp(test)) {
            op = test;
            cur_++;
            chars_++;
        } else {
            break;
        }
    }

    ops_++;
    tokens_.emplace_back(TOKEN_OPERATOR, op, lines_);
}

void Lexer::analyze() {
    std::string current;
    State return_state = RUNNING;
    int token_start_line = 1;

    while (buffer_[cur_] != EOF) {
        // debug
        printf("line: %d buffer: %lu\n char: %c", 
                lines_, buffer_.size(), buffer_[cur_]);
        char c = buffer_[cur_];
        
        switch (state_) {
        case RUNNING:
            if (std::isspace(c)) {
                if (c == '\n') lines_++;
                cur_++;
                chars_++;
            } else if (c == '#') {
                std::string prefix(buffer_.data() + cur_, 8);
                if (prefix.find("include") == 1) {
                    state_ = PARSE_INCLUDE;
                    parseInclude();
                    state_ = RUNNING;
                } else if (prefix.find("define") == 1 || 
                           prefix.find("ifdef") == 1 || 
                           prefix.find("endif") == 1) {
                    parseDefine();
                }
                // printf("char: %s\n", prefix.c_str());
            } else if (c == '/') {
                if (cur_+1 < buffer_.size()) {
                    if (buffer_[cur_+1] == '/') {
                        state_ = PARSE_COMMENT;
                        cur_ += 2;
                        chars_ += 2;
                        parseComment(false);
                        state_ = RUNNING;
                    } else if (buffer_[cur_+1] == '*') {
                        state_ = PARSE_COMMENT;
                        cur_ += 2;
                        chars_ += 2;
                        parseComment(true);
                        state_ = RUNNING;
                    } else {
                        goto checkOp;
                    }
                }
            } else if (c == '"' || c == '\'') {
                state_ = PARSE_CONSTSTR;
                char quote = c;
                cur_++;
                parseConstStr(quote);
                state_ = RUNNING;
            } else if (isLetter(c) || c == '_') {
                state_ = PARSE_WORD;
                parseWord();
                state_ = RUNNING;
            } else if (isDigit(c)) {
                state_ = PARSE_NUM;
                parseNumber();
                state_ = RUNNING;
            } else if (isOpChar(c)) {
            checkOp:
                state_ = PARSE_OP;
                parseOperator();
                state_ = RUNNING;
            } else if (isWhiteSpace(c)) {
                if (c == '\n') {
                    lines_++;
                }
                cur_++;
                chars_++;
            } else {
                tokens_.emplace_back(TOKEN_ERROR, std::string(1,c), lines_);
                cur_++;
                chars_++;
            }
            break;

        case PARSE_INCLUDE:
            break;
        case PARSE_CONSTSTR:
            break;
        case PARSE_WORD:
            break;
        case PARSE_NUM:
            break;
        case PARSE_OP:
            break;
        case PARSE_COMMENT:
            break;
        }
    }
    tokens_.emplace_back(TOKEN_EOF, "", lines_);
}

void Lexer::ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer_.resize(size + 1);
    file.read(buffer_.data(), size);
    buffer_[size] = EOF;  
}

void Lexer::StartAnalyse() {
    state_ = RUNNING;
    analyze();
}

void Lexer::PrintTokens() const {
    constexpr int type_width = 15;
    constexpr int value_width = 20;
    constexpr int line_width = 6;

    std::cout << "\n==== Token List ====\n";
    std::cout << std::left 
        << std::setw(type_width) << "Token Type"
        << std::setw(value_width) << "Value"
        << std::setw(line_width) << "Line"
        << "\n";
    std::cout << std::string(type_width + value_width + line_width + 2, '-') 
            << std::endl;

    for (const auto& token : tokens_) {
        std::string type_str;
        switch (token.type_) {
            case TOKEN_KEYWORD:     type_str = "KEYWORD"; break;
            case TOKEN_IDENTIFIER:  type_str = "IDENTIFIER"; break;
            case TOKEN_NUMBER:      type_str = "NUMBER"; break;
            case TOKEN_OPERATOR:    type_str = "OPERATOR"; break;
            case TOKEN_ASSIGNMENT:  type_str = "ASSIGNMENT"; break;
            case TOKEN_PUNCTUATION: type_str = "PUNCTUATION"; break;
            case TOKEN_COMMENT:     type_str = "COMMENT"; break;
            case TOKEN_EOF:         type_str = "EOF"; break;
            case TOKEN_ERROR:       type_str = "\033[31mERROR\033[0m"; break; // 红色高亮
            default:                type_str = "UNKNOWN";
        }

        std::cout << std::setw(type_width) << type_str
                  << std::setw(value_width) << token.value_
                  << std::setw(line_width) << token.line_
                  << "\n";
    }
}

 void Lexer::PrintInfo() const {
    std::cout << "\n==== Analysis Report ====\n";
    std::cout << std::left 
        << std::setw(20) << "Total lines:" << lines_ << "\n"
        << std::setw(20) << "Total characters:" << chars_ << "\n"
        << std::setw(20) << "Keywords found:" << keywords_ << "\n"
        << std::setw(20) << "Identifiers:" << ids_ << "\n"
        << std::setw(20) << "Operators:" << ops_ << "\n"
        << std::setw(20) << "Comments:" << comments_ << "\n"
        << std::setw(20) << "Total tokens:" << tokens_.size() << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file.c>\n";
        return 1;
    }

    Lexer lexer;
    
    try {
        lexer.ReadFile(argv[1]);
        lexer.StartAnalyse();
        lexer.PrintTokens();
        lexer.PrintInfo();
        
        bool has_error = false;
        for (const auto& t : lexer.GetTokens()) {
            if (t.type_ == Lexer::TOKEN_ERROR) {
                has_error = true;
                break;
            }
        }
        
        if (has_error) {
            std::cerr << "\n\033[31m[!] Analysis completed with errors\033[0m\n";
            return 2;
        }
        
        std::cout << "\n\033[32m[✓] Analysis completed successfully\033[0m\n";
    } 
    catch (const std::exception& e) {
        std::cerr << "\n\033[31m[!] Fatal error: " << e.what() << "\033[0m\n";
        return 3;
    }

    return 0;
}