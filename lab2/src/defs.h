#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iomanip>
#include <list>
#include <queue>
#include <set>


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

    bool operator<(const Symbol& other) const {
        return this->self_ > other.self_;
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

    bool operator==(const Expression& other) const { return self_ == other.self_; }

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

namespace std {
template <>
struct hash<Expression> {
    std::size_t operator()(const Expression& e) const {
        std::size_t seed = 0;
        for (const auto& symbol : e.self()) {
            seed ^= std::hash<Symbol>{}(symbol) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
}

class Production {
private:
    Symbol left_;
    Expression right_;

public:    
    Production() : left_(EPSILON), right_(Expression({EPSILON})) {}
    Production(Symbol l, std::vector<Symbol> r) : left_(l), right_(r) {}

    bool operator==(const Production& other) const { return left_ == other.left_ && right_ == other.right_; }

    const Symbol& left() const { return this->left_; }
    const Expression& right() const { return this->right_; }
    std::string to_string() const {
        std::string res = left_.self();
        res += " -> " + right_.to_string();
        return res;
    }
};

namespace std {
template <>
struct hash<Production> {
    std::size_t operator()(const Production& p) const {
        auto h1 = std::hash<Symbol>{}(p.left());
        auto h2 = std::hash<Expression>{}(p.right());
        return h1 ^ (h2 << 1);
    }
};
}

enum RC {
    SUCCESS,
    FAILED,
    INVALID_ARGUMENT,
};

class Item {
private:
    Production prod_;
    int pos_;

public:
    Item() : prod_(), pos_(0) {}
    Item(Production p, int pos) : prod_(p), pos_(pos) {}

    bool operator==(const Item& other) const {
        return prod_ == other.prod_ && pos_ == other.pos_;
    }

    const Production& prod() const { return prod_; }
    int pos() const { return pos_; }
    std::string to_string() const {
        std::string res = prod_.left().self();
        res += " -> ";
        const auto& rhs = prod_.right().self();
        for (size_t i = 0; i < rhs.size(); ++i) {
            if (i == static_cast<size_t>(pos_)) {
                res += ".";
            }
            res += rhs[i].self();
        }
        if (static_cast<size_t>(pos_) == rhs.size()) {
            res += ".";
        }
        return res;
    }
};

namespace std {
template <>
struct hash<Item> {
    std::size_t operator()(const Item& item) const {
        auto h1 = std::hash<Production>{}(item.prod());
        auto h2 = std::hash<int>{}(item.pos());
        return h1 ^ (h2 << 1);
    }
};
}

class Identity {
private:
    int id_;
    std::unordered_set<Item> items_;

public:
    Identity(int id, std::unordered_set<Item> items) : id_(id), items_(items) {}

    bool cmp(const std::unordered_set<Item>& other) const {
        return this->items_ == other;
    }

    bool operator==(const Identity& other) const {
        return this->cmp(other.items_);
    }

    bool operator<(const Identity& other) const {
        return this->id() < other.id();
    }

    int id() const { return id_; }
    std::unordered_set<Item> items() const { return items_; }

    std::string to_string() {
        std::string res = "";
        for (auto i : items_) {
            res += i.to_string() + "\n";
        }
        return res;
    }
};

namespace std {
template <>
struct hash<Identity> {
    std::size_t operator()(const Identity& id) const {
        std::size_t seed = 0;
        for (const auto& item : id.items()) {
            seed ^= std::hash<Item>{}(item);
        }
        return seed;
    }
};
}

class Move {
public:
    enum class MoveType {
        Start,
        Shift,
        Reduce,
        End,
    };
private:
    int id_;
    MoveType type_;

public:
    Move() : id_(-1), type_(MoveType::Start) {}
    Move(int i, MoveType t) : id_(i), type_(t) {}
    int id() const { return id_; }
    MoveType type() const { return type_; }
};