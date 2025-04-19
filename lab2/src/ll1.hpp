#pragma once

#include "defs.h"
#include <string>
#include <unordered_set>

class ParserLL1 {
private:
    std::unordered_set<Symbol> deduce_empty;
    std::unordered_map<Symbol, std::unordered_set<Symbol>> first_set;
    std::unordered_map<Symbol, std::unordered_set<Symbol>> follow_set;
    std::unordered_map<Symbol, std::unordered_map<Symbol, Production>> table;

    // parse original input and transform them into self attributes
    RC eliminate_left_recursion(std::vector<Production> input, std::vector<Production>& res);
    // parse first sets
    RC parse_first(std::vector<Production>& res);
    // parse follow sets
    RC parse_follow(std::vector<Production>& res);
    // help function that compute first set of that symbol
    std::unordered_set<Symbol> compute_first_sequence(const std::vector<Symbol>& symbols);
    // make table
    RC build_table(std::vector<Production>& res);

public:
    // finish parse phase and table building during here
    ParserLL1(std::vector<Production> input);    
    ~ParserLL1();

    RC analyse(std::vector<Symbol>& input);    

    const std::unordered_map<Symbol, std::unordered_set<Symbol>>& get_first_set() const { return first_set; }
    const std::unordered_map<Symbol, std::unordered_set<Symbol>>& get_follow_set() const { return follow_set; }
    const std::unordered_map<Symbol, std::unordered_map<Symbol, Production>>& get_table() const { return table; }

    void print_sets_and_table() const; 
};
