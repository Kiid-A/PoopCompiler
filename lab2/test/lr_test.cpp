#include "../src/lr.hpp"


bool is_integer(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

int main() {
    Symbol E("E", false), T("T", false), F("F", false);
    Symbol plus("+", true), mult("*", true), minus("-", true), 
           divide("/", true), lparen("(", true), rparen(")", true), 
           num("num", true);

    std::vector<Production> prods = {
        Production(E, {E, plus, T}),
        Production(E, {E, minus, T}),
        Production(E, {T}),
        Production(T, {T, mult, F}),
        Production(T, {T, divide, F}),
        Production(T, {F}),
        Production(F, {lparen, E, rparen}),
        Production(F, {num}),
    };

    ParserLR parser(prods);
    parser.print_tables();

    std::string input_str = "3 + 4 * 5";
    std::istringstream iss(input_str);
    std::string token;
    std::vector<Symbol> input;
    while (iss >> token) {
        if (is_integer(token)) {
            input.push_back(num);
        } else {
            input.emplace_back(token, true);
        }
    }

    RC result = parser.analyse(input);

    if (result == RC::SUCCESS) {
        std::cout << "Input is valid!" << std::endl;
    } else {
        std::cout << "Input is invalid!" << std::endl;
    }

    return 0;
}