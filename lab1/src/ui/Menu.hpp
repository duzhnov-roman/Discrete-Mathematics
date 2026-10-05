#pragma once
#include "core/GrayCode.hpp"
#include "core/Multiset.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class Menu{
    std::shared_ptr<const CodeSet> carrier;   // общий носитель U, A и B
    Multiset universe;
    Multiset a;
    Multiset b;
    bool universe_ready;
    bool a_ready;
    bool b_ready;

    bool is_available(char command) const;

    void create_universe();
    void fill_multiset(Multiset& multiset, bool& ready, const std::string& name);
    bool fill_manual(Multiset& target, size_t cardinality, const std::string& name) const;
    bool parse_manual_line(const std::string& line, const Multiset& target, size_t left,
                           std::vector<size_t>& indices, std::vector<size_t>& counts) const;

    std::vector<Multiset> compute_operations() const;
    void show_help() const;
    void show_table() const;
    void show_operations() const;
    void show_matrix() const;

public:
    Menu();
    void run();
};
