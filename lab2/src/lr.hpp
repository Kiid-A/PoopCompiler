#include "defs.h"


class ParserLR {
private:
    std::unordered_set<Identity> identity_set_;    
    std::unordered_set<Production> prods_;
    std::vector<Production> augmented_prods_;
    std::unordered_map<int, std::unordered_map<Symbol, Move>> action_table_;
    std::unordered_map<int, std::unordered_map<Symbol, int>> goto_table_;
    std::unordered_map<Symbol, std::unordered_set<Symbol>> follow_set;

    // simply add S
    std::vector<Production> augment_grammar(const std::vector<Production>& input);
    // calc closure of this Identity
    std::unordered_set<Item> closure(const std::unordered_set<Item>& items);
    // calc goto table
    std::unordered_set<Item> goto_function(const std::unordered_set<Item>& items, const Symbol& symbol);
    // construct Identity set
    void construct_canonical_collection(const std::vector<Production>& augmented_prods);
    // 
    void build_tables(const std::vector<Production>& augmented_prods);

public:
    ParserLR(std::vector<Production> p);
    ~ParserLR() {}

    RC analyse(std::vector<Symbol>& input);

    void print_tables() const;
};