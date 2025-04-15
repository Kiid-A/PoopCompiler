#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iomanip>
#include <list>



class Symbol;
class Production;

class Symbol {
private:
    std::string self_;
    bool is_terminal_;

public:
    Symbol() : self_(""), is_terminal_(true) {} 
    Symbol(std::string s, bool i) 
        : self_(s),
          is_terminal_(i)
    {}
    
    bool operator==(const Symbol& other) const { 
        return this->self_ == other.self_ 
            && this->is_terminal_ == other.is_terminal_; 
    }

    std::string self() const { return self_; }
    bool is_terminal() const { return is_terminal_; }
    bool cmp(const Symbol& other) const {
        return this->self_ == other.self_ 
            && this->is_terminal_ == other.is_terminal_; 
    }
};

namespace std {
template <>
struct hash<Symbol> {
    std::size_t operator()(const Symbol& s) const {
        auto h1 = std::hash<std::string>{} (s.self());
        auto h2 = std::hash<bool>{} (s.is_terminal());
        return h1 ^ (h2 << 1);
    }
};
}

const Symbol END = Symbol("$", true);
const Symbol EPSILON = Symbol("@", true);

class Expression {
private:
    std::vector<Symbol> self_;

public:
    Expression() : self_() {}
    Expression(std::vector<Symbol> s) : self_(s) {}

    const std::vector<Symbol>& self() const { return self_; }
    std::string to_string() const {
        std::string s = "";
        for (auto symbol : self_) {
            s += symbol.self();
        }
        return s;
    }
    Symbol operator[](int i) const {
        return self_[i];
    }

    void push_back(Symbol s) { self_.push_back(s); }
};

class Production {
private:
    Symbol left_;
    Expression right_;

public:    
    Production() : left_(EPSILON), right_(Expression({EPSILON})) {}
    Production(Symbol l, std::vector<Symbol> r) : left_(l), right_(r) {}

    const Symbol& left() const { return this->left_; }
    const Expression& right() const { return this->right_; }
};

enum RC {
    SUCCESS,
    FAILED,
    INVALID_ARGUMENT,
};