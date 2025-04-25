#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "../src/lex.h"
#include "../src/yacc.tab.h"


bool is_integer(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

void handle_lexical_error(const std::string& message) {
    std::cerr << "Lexical Error: " << message << std::endl;
    std::exit(EXIT_FAILURE);
}


int main() {
    std::string input_str = "3+2-2*5\0";

    YY_BUFFER_STATE buffer = yy_scan_string(input_str.c_str());
    if (!buffer) {
        handle_lexical_error("Failed to create buffer for input string.");
    }

    int result = yyparse();

    yy_delete_buffer(buffer);

    return 0;
}