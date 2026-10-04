#pragma once
#include "core/GrayCode.hpp"
#include "core/Multiset.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class Menu{
    struct Operation{
        std::string title;       // название операции
        std::string notation;    // обозначение, например A ∪ B
        Multiset result;
    };

    std::shared_ptr<const CodeSet> carrier;   // общий носитель U, A и B
    Multiset universe;
    Multiset a;
    Multiset b;
    bool universe_ready;
    bool a_ready;
    bool b_ready;

    void print_menu() const;
    bool require_universe() const;
    bool require_multisets() const;

    void create_universe();
    bool read_universe_manually(Multiset& target) const;
    void fill_multiset(Multiset& multiset, bool& ready, const std::string& name);
    bool fill_manual(Multiset& target, size_t cardinality, const std::string& name) const;

    std::vector<Operation> compute_operations() const;
    void show_table(bool with_multisets) const;
    void show_operations() const;
    void show_operations_table() const;

public:
    Menu();
    void run();
};
