#include "ll1.hpp"
#include "defs.h"
#include <algorithm>
#include <cstdio>
#include <queue>


RC ParserLL1::eliminate_left_recursion(std::vector<Production> input, std::vector<Production>& res) {
    auto is_left_recursion = [](const Production& p) -> bool {
        return !p.right().self().empty() && 
               p.left().self() == p.right()[0].self() && 
               !p.right()[0].is_terminal();
    };

    // Add S->E
    res.insert(res.begin(), {{Symbol("S", false), {input[0].left()}}});

    std::unordered_map<std::string, std::vector<Production>> productions;
    for (auto& p : input) {
        productions[p.left().self()].push_back(p);
    }

    for (auto& prod_pair : productions) {
        const std::string& A_name = prod_pair.first;
        // printf("A name: %s\n", A_name.c_str());
        auto& prods = prod_pair.second;

        std::vector<Production> lr_prod, non_lr_prod;
        for (auto& p : prods) {
            if (is_left_recursion(p)) {
                std::vector<Symbol> right(p.right().self().begin() + 1, p.right().self().end());
                lr_prod.emplace_back(p.left(), right);
            } else {
                non_lr_prod.emplace_back(p.left(), p.right().self());
            }
        }

        if (!lr_prod.empty()) {
            Symbol A_prime(A_name + "\'", false);
            
            for (auto& prod : non_lr_prod) {
                Expression new_right = prod.right();
                new_right.push_back(A_prime);
                res.emplace_back(prod.left(), new_right.self());
                // printf("Add new prod: %s -> %s \n", prod.left().self().c_str(), new_right.to_string().c_str());
            }
            
            for (auto& prod : lr_prod) {
                Expression new_right = prod.right();
                new_right.push_back(A_prime);
                res.emplace_back(A_prime, new_right.self());
            }
            
            res.emplace_back(A_prime, std::vector<Symbol>{EPSILON});
        } else {
            res.insert(res.end(), prods.begin(), prods.end());
        }
    }

    return RC::SUCCESS;
}

RC ParserLL1::parse_first(std::vector<Production>& input) {
    std::function<std::unordered_set<Symbol>(const Symbol&)> compute_first;
    compute_first = [&](const Symbol& symbol) -> std::unordered_set<Symbol> {
        std::unordered_set<Symbol> result;
        if (symbol.is_terminal()) {
            result.insert(symbol);
        } else {
            for (auto& p : input) {
                if (p.left().cmp(symbol) && !p.right().self().empty()) {
                    auto& first = first_set[symbol];
                    bool has_epsilon = true;
                    for (const auto& s : p.right().self()) {
                        auto s_first = compute_first(s);
                        result.insert(s_first.begin(), s_first.end());
                        result.erase(EPSILON);
                        if (s_first.find(EPSILON) == s_first.end()) {
                            has_epsilon = false;
                            break;
                        }
                    }
                    if (has_epsilon) {
                        result.insert(EPSILON);
                    }
                }
            }
        }
        return result;
    };

    bool changed;
    do {
        changed = false;
        for (auto& p : input) {
            auto A = p.left();
            auto old_size = first_set[A].size();
            for (const auto& s : compute_first(p.left())) {
                first_set[A].insert(s);
            }
            if (first_set[A].size() != old_size) {
                changed = true;
            }
        }
    } while (changed);

    return RC::SUCCESS;
}

std::unordered_set<Symbol> ParserLL1::compute_first_sequence(const std::vector<Symbol>& symbols) {
    std::unordered_set<Symbol> result;
    bool can_deduce_epsilon = true;

    for (const Symbol& s : symbols) {
        if (s.is_terminal()) {
            result.insert(s);
            can_deduce_epsilon = false;
            break;
        } else {
            const auto& first = first_set[s];
            result.insert(first.begin(), first.end());
            result.erase(EPSILON);

            if (first.find(EPSILON) == first.end()) {
                can_deduce_epsilon = false;
                break;
            }
        }
    }

    if (can_deduce_epsilon) {
        result.insert(EPSILON);
    }

    return result;
}

RC ParserLL1::parse_follow(std::vector<Production>& input) {
    follow_set[input[0].left()].insert(END);

    bool changed;
    do {
        changed = false;
        for (auto& p : input) {
            const auto& A = p.left();
            const auto& rhs = p.right().self();
            for (size_t i = 0; i < rhs.size(); ++i) {
                if (rhs[i].is_terminal()) continue;

                auto& B = rhs[i];
                auto& follow_B = follow_set[B];
                size_t old_size = follow_B.size();

                if (i + 1 < rhs.size()) {
                    std::vector<Symbol> beta(rhs.begin() + i + 1, rhs.end());
                    auto first_beta = compute_first_sequence(beta);
                    follow_B.insert(first_beta.begin(), first_beta.end());
                    follow_B.erase(EPSILON);

                    if (first_beta.find(EPSILON) != first_beta.end()) {
                        const auto& follow_A = follow_set[A];
                        follow_B.insert(follow_A.begin(), follow_A.end());
                    }
                } else {
                    const auto& follow_A = follow_set[A];
                    follow_B.insert(follow_A.begin(), follow_A.end());
                }

                if (follow_B.size() != old_size) changed = true;
            }
        }
    } while (changed);

    return RC::SUCCESS;
}

RC ParserLL1::build_table(std::vector<Production>& input) {
    for (auto& p : input) {
        const auto& A = p.left();
        const auto& alpha = p.right();

        auto first_alpha = compute_first_sequence(alpha.self());
        for (const auto& a : first_alpha) {
            if (!a.cmp(EPSILON)) {
                table[A][a] = p;
            }
        }

        if (first_alpha.find(EPSILON) != first_alpha.end()) {
            for (const auto& b : follow_set[A]) {
                table[A][b] = p;
            }
        }
    }
    return RC::SUCCESS;
}

ParserLL1::ParserLL1(std::vector<Production> input) {
    std::vector<Production> new_prods;
    printf("start building parser\n");
    eliminate_left_recursion(input, new_prods);
    printf("finished eliminate_left_recursion\n");
    parse_first(new_prods);
    printf("finished parse_first\n");
    parse_follow(new_prods);
    printf("finished parse_follow\n");
    build_table(new_prods);
}

ParserLL1::~ParserLL1() {}

// RC ParserLL1::analyse(std::vector<Symbol>& input) {
//     input.push_back(END);
//     std::vector<Symbol> stack = {END, Symbol("S", false)};

//     size_t index = 0;
//     while (!stack.empty()) {
//         Symbol top = stack.back();
//         printf("top: %s, ", top.self().c_str());
//         stack.pop_back();
//         if (top.cmp(END)) {
//             if (input[index].cmp(END)) return RC::SUCCESS;
//             else return RC::FAILED;
//         }

//         if (top.is_terminal()) {
//             if (top.cmp(input[index])) {
//                 ++index;
//                 printf("\n");
//             } else {
//                 return RC::FAILED;
//             }
//         } else {
//             const auto& entry = table[top];
//             auto it = entry.find(input[index]);
//             if (it == entry.end()) return RC::FAILED;

//             const auto& prod = it->second;
//             const auto& rhs = prod.right().self();
//             printf("use: %s -> %s\n", prod.left().self().c_str(), prod.right().to_string().c_str());
//             if (rhs.size() == 1 && rhs[0].cmp(EPSILON)) continue;
//             for (auto it = rhs.rbegin(); it != rhs.rend(); ++it) {
//                 stack.push_back(*it);
//             }
//         }
//     }
//     return RC::FAILED;
// }

RC ParserLL1::analyse(std::vector<Symbol>& input) {
    input.push_back(END);
    std::vector<Symbol> stack = {END, Symbol("S", false)};

    size_t index = 0;
    std::cout << "LL(1) Parsing Process:" << std::endl;
    std::cout << std::left << std::setw(20) << "Stack" << std::setw(20) << "Input" << std::setw(20) << "Action" << std::endl;
    while (!stack.empty()) {
        Symbol top = stack.back();
        stack.pop_back();

        std::string stack_str = "";
        for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
            stack_str += it->self() + " ";
        }
        stack_str += top.self();

        std::string input_str = "";
        for (size_t i = index; i < input.size(); ++i) {
            input_str += input[i].self() + " ";
        }

        std::string action_str;
        if (top.cmp(END)) {
            if (input[index].cmp(END)) {
                action_str = "Accept";
                std::cout << std::left << std::setw(20) << stack_str << std::setw(20) << input_str << std::setw(20) << action_str << std::endl;
                return RC::SUCCESS;
            } else {
                action_str = "Error";
                std::cout << std::left << std::setw(20) << stack_str << std::setw(20) << input_str << std::setw(20) << action_str << std::endl;
                return RC::FAILED;
            }
        }

        if (top.is_terminal()) {
            if (top.cmp(input[index])) {
                action_str = "Match " + top.self();
                ++index;
            } else {
                action_str = "Error: Mismatch " + top.self() + " and " + input[index].self();
                std::cout << std::left << std::setw(20) << stack_str << std::setw(20) << input_str << std::setw(20) << action_str << std::endl;
                return RC::FAILED;
            }
        } else {
            const auto& entry = table[top];
            auto it = entry.find(input[index]);
            if (it == entry.end()) {
                action_str = "Error: No production for " + top.self() + " on " + input[index].self();
                std::cout << std::left << std::setw(20) << stack_str << std::setw(20) << input_str << std::setw(20) << action_str << std::endl;
                return RC::FAILED;
            }

            const auto& prod = it->second;
            const auto& rhs = prod.right().self();
            action_str = "Use production: " + prod.to_string();
            if (rhs.size() == 1 && rhs[0].cmp(EPSILON)) continue;
            for (auto it = rhs.rbegin(); it != rhs.rend(); ++it) {
                stack.push_back(*it);
            }
        }
        std::cout << std::left << std::setw(20) << stack_str << std::setw(20) << input_str << std::setw(20) << action_str << std::endl;
    }
    std::cout << std::left << std::setw(20) << "" << std::setw(20) << "" << std::setw(20) << "Error: Stack empty prematurely" << std::endl;
    return RC::FAILED;
}

void ParserLL1::print_sets_and_table() const {
    std::cout << "First Sets:" << std::endl;
    for (const auto& pair : first_set) {
        std::cout << "First(" << pair.first.self() << ") = { ";
        for (const auto& symbol : pair.second) {
            std::cout << symbol.self() << " ";
        }
        std::cout << "}" << std::endl;
    }

    std::cout << "\nFollow Sets:" << std::endl;
    for (const auto& pair : follow_set) {
        std::cout << "Follow(" << pair.first.self() << ") = { ";
        for (const auto& symbol : pair.second) {
            std::cout << symbol.self() << " ";
        }
        std::cout << "}" << std::endl;
    }

    std::cout << "\nParsing Table:" << std::endl;
    for (const auto& row : table) {
        for (const auto& cell : row.second) {
            std::cout << "M[" << row.first.self() << ", " << cell.first.self() << "] = " 
                      << row.first.self() << " -> " << cell.second.right().to_string() << std::endl;
        }
    }
    std::cout << "\n";
}