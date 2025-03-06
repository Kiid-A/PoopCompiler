#include <iostream>
#include <fstream>
#include "lexer.h"

void Lexer::parseComment(bool isBlock) {
    std::string comment;
    int start_line = lines_;
    char prev = 0;

    while (cur_ < buffer_.size()) {
        char c = buffer_[cur_++];
        chars_++;

        if (c == '\n') lines_++;

        if (isBlock) {
            if (prev == '*' && c == '/') {
                comments_++;
                return;
            }
            prev = c;
        } else if (c == '\n') {
            comments_++;
            return;
        }

        comment += c;
    }

    if (isBlock) {
        tokens_.emplace_back(TOKEN_ERROR, "Unclosed block comment", start_line);
    }
}

void Lexer::parseInclude() {

}

// 完整的状态转移实现
void Lexer::analyze() {
    std::string current;
    State return_state = RUNNING;
    int token_start_line = 1;

    while (cur_ < buffer_.size()) {
        char c = buffer_[cur_];
        
        switch (state_) {
        case RUNNING:
            if (std::isspace(c)) {
                if (c == '\n') lines_++;
                cur_++;
                chars_++;
            } else if (c == '')else if (c == '/') {
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
                    }
                }
            } else if (c == '"' || c == '\'') {
                state_ = PARSE_CONSTSTR;
                
            }
            // 其他状态转换...
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

void Lexer::PrintInfo() const {
    std::cout << "==== Analysis Report ====\n"
              << "Total lines: " << lines_ << "\n"
              << "Total characters: " << chars_ << "\n"
              << "Keywords: " << keywords_ << "\n"
              << "Identifiers: " << ids_ << "\n"
              << "Operators: " << ops_ << "\n"
              << "Comments: " << comments_ << "\n";
}

int main(int argc, char **argv)
{
    std::string filename;
    if (argc > 1) filename = argv[1];
    else printf("error:\n command: lexC filename");
    
    return 0;
}