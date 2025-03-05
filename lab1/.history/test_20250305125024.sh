#!/bin/bash

# 检查是否提供了文件夹路径
if [ "$#1" -eq 0 ]; then
    echo "Usage: $0 <test_folder_path>"
    exit 1
fi

TEST_FOLDER=$1

# 检查文件夹是否存在
if [ ! -d "$TEST_FOLDER" ]; then
    echo "Error: Directory $TEST_FOLDER does not found."
    exit 1
fi

# 遍历文件夹中的所有文件
for file in "$TEST_FOLDER"/*.c; do
    if [ -f "$file" ]; then
        echo "Running lexer on $file..."
        ./lex "$file"
        if [ $? -eq 0 ]; then
            echo "Lexer completed successfully for $file"
        else
            echo "Lexer failed for $file"
        fi
    else
        echo "Skipping $file, not a regular file."
    fi
done

echo "All files processed."