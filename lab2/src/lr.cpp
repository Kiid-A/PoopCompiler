#include "lr.hpp"
#include "defs.h"
#include <cstdio>
#include <iostream>
#include <ostream>
#include <unordered_set>


std::unordered_set<Symbol> first(const std::vector<Symbol>& symbols, const std::unordered_set<Production>& prods) {
    std::unordered_set<Symbol> result;
    for (const auto& symbol : symbols) {
        if (symbol.is_terminal()) {
            result.insert(symbol);
            break;
        } else {
            bool has_epsilon = false;
            for (const auto& prod : prods) {
                if (prod.left().cmp(symbol)) {
                    const auto& rhs = prod.right().self();
                    auto first_rhs = first(rhs, prods);
                    for (const auto& s : first_rhs) {
                        if (s.cmp(EPSILON)) {
                            has_epsilon = true;
                        } else {
                            result.insert(s);
                        }
                    }
                }
            }
            if (!has_epsilon) {
                break;
            }
        }
    }
    return result;
}

std::unordered_map<Symbol, std::unordered_set<Symbol>> follow(const std::vector<Production>& augmented_prods, const std::unordered_set<Production>& prods) {
    std::unordered_map<Symbol, std::unordered_set<Symbol>> follow_set;
    Symbol S("S", false);
    follow_set[S].insert(END);

    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& prod : augmented_prods) {
            const Symbol& left = prod.left();
            const auto& rhs = prod.right().self();
            for (size_t i = 0; i < rhs.size(); ++i) {
                const Symbol& symbol = rhs[i];
                if (!symbol.is_terminal()) {
                    if (i < rhs.size() - 1) {
                        std::vector<Symbol> rest(rhs.begin() + i + 1, rhs.end());
                        auto first_rest = first(rest, prods);
                        for (const auto& s : first_rest) {
                            if (!s.cmp(EPSILON)) {
                                if (follow_set[symbol].find(s) == follow_set[symbol].end()) {
                                    follow_set[symbol].insert(s);
                                    changed = true;
                                }
                            }
                        }
                        if (first_rest.find(EPSILON) != first_rest.end()) {
                            for (const auto& s : follow_set[left]) {
                                if (follow_set[symbol].find(s) == follow_set[symbol].end()) {
                                    follow_set[symbol].insert(s);
                                    changed = true;
                                }
                            }
                        }
                    } else {
                        for (const auto& s : follow_set[left]) {
                            if (follow_set[symbol].find(s) == follow_set[symbol].end()) {
                                follow_set[symbol].insert(s);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return follow_set;
}

ParserLR::ParserLR(std::vector<Production> p) {
    printf("LR parse started\n");
    prods_ = std::unordered_set<Production>(p.begin(), p.end());
    augmented_prods_ = augment_grammar(p);
    printf("construct start\n");
    construct_canonical_collection(augmented_prods_);
    printf("build table start\n");
    build_tables(augmented_prods_);
}

std::vector<Production> ParserLR::augment_grammar(const std::vector<Production>& input) {
    std::vector<Production> augmented_prods = input;
    Symbol S_prime("S", false);
    augmented_prods.insert(augmented_prods.begin(), Production(S_prime, {input[0].left()}));
    return augmented_prods;
}

std::unordered_set<Item> ParserLR::closure(const std::unordered_set<Item>& items) {
    std::unordered_set<Item> result = items;
    std::queue<Item> worklist;

    for (const auto& item : items) {
        worklist.push(item);
    }

    while (!worklist.empty()) {
        Item current = worklist.front();
        worklist.pop();

        const auto& rhs = current.prod().right().self();
        int dot_pos = current.pos();
        if (dot_pos < static_cast<int>(rhs.size()) && !rhs[dot_pos].is_terminal()) {
            for (const auto& prod : prods_) {
                if (prod.left().cmp(rhs[dot_pos])) {
                    Item new_item(prod, 0);
                    if (result.find(new_item) == result.end()) {
                        result.insert(new_item);
                        worklist.push(new_item);
                    }
                }
            }
        }
    }

    return result;
}

std::unordered_set<Item> ParserLR::goto_function(const std::unordered_set<Item>& items, const Symbol& symbol) {
    std::unordered_set<Item> result;
    for (const auto& item : items) {
        const auto& rhs = item.prod().right().self();
        int dot_pos = item.pos();
        if (dot_pos < static_cast<int>(rhs.size()) && rhs[dot_pos].cmp(symbol)) {
            Item new_item(item.prod(), dot_pos + 1);
            result.insert(new_item);
        }
    }
    return closure(result);
}

void ParserLR::construct_canonical_collection(const std::vector<Production>& augmented_prods) {
    std::queue<Identity> worklist;
    std::set<Identity> visited;

    Item start_item(augmented_prods[0], 0);
    std::unordered_set<Item> start_items = {start_item};
    Identity start_identity(0, closure(start_items));
    identity_set_.insert(start_identity);
    worklist.push(start_identity);

    int next_id = 1;

    while (!worklist.empty()) {
        Identity current = worklist.front();
        worklist.pop();

        std::unordered_set<Symbol> symbols;
        for (const auto& item : current.items()) {
            const auto& rhs = item.prod().right().self();
            int dot_pos = item.pos();
            if (dot_pos < static_cast<int>(rhs.size())) {
                symbols.insert(rhs[dot_pos]);
            }
        }

        for (const auto& symbol : symbols) {
            std::unordered_set<Item> next_items = goto_function(current.items(), symbol);
            if (!next_items.empty()) {
                Identity next_identity(0, next_items);
                auto it = identity_set_.find(next_identity);
                if (it == identity_set_.end()) {
                    next_identity = Identity(next_id++, next_items);
                    identity_set_.insert(next_identity);
                    worklist.push(next_identity);
                } else {
                    next_identity = *it;
                }
                if (symbol.is_terminal()) {
                    action_table_[current.id()][symbol] = Move(next_identity.id(), Move::MoveType::Jump);
                } else {
                    goto_table_[current.id()][symbol] = next_identity.id();
                }
            }
        }
        visited.insert(current);
    }
    int cnt = 0;
    for (auto id : visited) {
        std::cout << "Identity: " << cnt++ << "\n" << id.to_string() << "\n";
    }
}

void ParserLR::build_tables(const std::vector<Production>& augmented_prods) {
    follow_set = follow(augmented_prods, prods_);
    for (const auto& identity : identity_set_) {
        for (const auto& item : identity.items()) {
            const auto& rhs = item.prod().right().self();
            int dot_pos = item.pos();
            if (dot_pos == rhs.size()) {
                if (item.prod().left().self() == "S") {
                    action_table_[identity.id()][END] = Move(-1, Move::MoveType::End);
                } else {
                    int prod_index = std::find(augmented_prods.begin(), augmented_prods.end(), item.prod()) - augmented_prods.begin();
                    for (const auto& follow_symbol : follow_set[item.prod().left()]) {
                        action_table_[identity.id()][follow_symbol] = Move(prod_index, Move::MoveType::Reduce);
                    }
                }
            }
        }
    }
}

RC ParserLR::analyse(std::vector<Symbol>& input) {
    input.push_back(END);
    std::vector<int> state_stack = {0};
    std::vector<Symbol> symbol_stack = {END};
    size_t index = 0;

    while (true) {
        int current_state = state_stack.back();
        Symbol current_symbol = input[index];
        std::cout << "current: " << current_symbol.self() << ", ";
        auto action_it = action_table_[current_state].find(current_symbol);
        if (action_it == action_table_[current_state].end()) {
            return RC::FAILED;
        }

        Move move = action_it->second;
        switch (move.type()) {
            case Move::MoveType::Jump:
                std::cout << "jump: " << move.id() << std::endl;
                state_stack.push_back(move.id());
                symbol_stack.push_back(current_symbol);
                index++;
                break;
            case Move::MoveType::Reduce: {
                std::cout << "reduce: " << move.id() << std::endl;
                Production prod = augmented_prods_[move.id()];
                const auto& rhs = prod.right().self();
                for (size_t i = 0; i < rhs.size(); ++i) {
                    state_stack.pop_back();
                    symbol_stack.pop_back();
                }
                int prev_state = state_stack.back();
                Symbol left = prod.left();
                auto goto_it = goto_table_[prev_state].find(left);
                if (goto_it == goto_table_[prev_state].end()) {
                    return RC::FAILED;
                }
                int next_state = goto_it->second;
                state_stack.push_back(next_state);
                symbol_stack.push_back(left);
                break;
            }
            case Move::MoveType::End:
                if (input[index].cmp(END)) {
                    return RC::SUCCESS;
                } else {
                    return RC::FAILED;
                }
            default:
                return RC::FAILED;
        }
    }
    return RC::FAILED;
}

void ParserLR::print_tables() const {
    int cnt = 0;
    for (auto p : augmented_prods_) {
        std::cout << "Production: " << ++cnt << " " << p.to_string() << "\n";
    }

    std::set<Symbol> action_symbols;
    for (const auto& row : action_table_) {
        for (const auto& cell : row.second) {
            if (cell.first.is_terminal()) {
                action_symbols.insert(cell.first);
            }
        }
    }
    std::vector<Symbol> action_columns(action_symbols.begin(), action_symbols.end());
    std::sort(action_columns.begin(), action_columns.end(), [](const Symbol& a, const Symbol& b) {
        return a.self() < b.self();
    });

    std::set<Symbol> goto_symbols;
    for (const auto& row : goto_table_) {
        for (const auto& cell : row.second) {
            goto_symbols.insert(cell.first);
        }
    }
    std::vector<Symbol> goto_columns(goto_symbols.begin(), goto_symbols.end());
    std::sort(goto_columns.begin(), goto_columns.end(), [](const Symbol& a, const Symbol& b) {
        return a.self() < b.self();
    });

    std::set<int> all_states;
    for (const auto& row : action_table_) {
        all_states.insert(row.first);
    }
    for (const auto& row : goto_table_) {
        all_states.insert(row.first);
    }
    std::vector<int> states(all_states.begin(), all_states.end());
    std::sort(states.begin(), states.end());

    std::cout << "\nAction Table:\n";
    std::cout << std::left;

    std::cout << std::setw(6) << "State";
    for (const auto& sym : action_columns) {
        std::cout << std::setw(8) << sym.self();
    }
    std::cout << "\n";

    for (int state : states) {
        std::cout << std::setw(6) << state;
        for (const auto& sym : action_columns) {
            std::string cell;
            auto state_it = action_table_.find(state);
            if (state_it != action_table_.end()) {
                const auto& sym_map = state_it->second;
                auto sym_it = sym_map.find(sym);
                if (sym_it != sym_map.end()) {
                    const Move& m = sym_it->second;
                    switch (m.type()) {
                        case Move::MoveType::Jump:
                            cell = "s" + std::to_string(m.id());
                            break;
                        case Move::MoveType::Reduce:
                            cell = "r" + std::to_string(m.id());
                            break;
                        case Move::MoveType::End:
                            cell = "acc";
                            break;
                        default:
                            break;
                    }
                }
            }
            std::cout << std::setw(8) << cell;
        }
        std::cout << "\n";
    }

    std::cout << "\nGoto Table:\n";
    std::cout << std::setw(6) << "State";
    for (const auto& sym : goto_columns) {
        std::cout << std::setw(8) << sym.self();
    }
    std::cout << "\n";

    for (int state : states) {
        std::cout << std::setw(6) << state;
        for (const auto& sym : goto_columns) {
            std::string cell;
            auto state_it = goto_table_.find(state);
            if (state_it != goto_table_.end()) {
                const auto& sym_map = state_it->second;
                auto sym_it = sym_map.find(sym);
                if (sym_it != sym_map.end()) {
                    cell = std::to_string(sym_it->second);
                }
            }
            std::cout << std::setw(8) << cell;
        }
        std::cout << "\n";
    }
}