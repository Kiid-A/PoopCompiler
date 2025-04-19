#include "../src/lr.hpp"

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

    std::vector<Symbol> input = {num, plus, num, mult, num};
    RC result = parser.analyse(input);

    if (result == RC::SUCCESS) {
        std::cout << "Input is valid!" << std::endl;
    } else {
        std::cout << "Input is invalid!" << std::endl;
    }

    return 0;
}

