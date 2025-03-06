#include <iostream>
#include "lexer.h"

void Lexer::parseComment(bool isBlock)
{
    std::string comment;
    int start_line = lines_;
    char prev = 0;

    while (cur_ < buffer_.size())
    {
        char c = buffer_[cur_++];
        chars_++;

        if (c == '\n')
            lines_++;

        // 处理块注释结束
        if (isBlock)
        {
            if (prev == '*' && c == '/')
            {
                comments_++;
                return;
            }
            prev = c;
        }
        // 处理行注释结束
        else if (c == '\n')
        {
            comments_++;
            return;
        }

        comment += c;
    }

    // 未闭合注释处理
    if (isBlock)
    {
        tokens_.emplace_back(TOKEN_ERROR, "Unclosed block comment", start_line);
    }
}

int main(int argc, char **argv)
{
    std::string filename;
    if (argc > 1)
        filename = argv[1];
    else
        printf("error:\n command: lexC filename");

    return 0;
}