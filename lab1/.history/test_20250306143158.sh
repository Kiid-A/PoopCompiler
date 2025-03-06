#!/bin/bash

TEST_FOLDER="./test"

# 检查文件夹是否存在
if [ ! -d "$TEST_FOLDER" ]; then
    echo "Error: Directory $TEST_FOLDER does not found."
    exit 1
fi

# 遍历文件夹中的所有文件
for file in "$TEST_FOLDER"/*.c; do
    if [ -f "$file" ]; then
        echo "Running lexer on $file..."
        ./lexer "$file"
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