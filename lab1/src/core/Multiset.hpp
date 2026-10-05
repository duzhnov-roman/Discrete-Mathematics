#pragma once
#include "core/GrayCode.hpp"
#include <cstddef>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>


class MultisetError : public std::runtime_error{
public:
    explicit MultisetError(const std::string& message):std::runtime_error(message){}
};

class MultisetIndexError : public MultisetError{
public:
    MultisetIndexError():MultisetError("номер элемента вне носителя"){}
};

class CarrierMismatchError : public MultisetError{
public:
    CarrierMismatchError():MultisetError("мультимножества построены над разными носителями"){}
};

class NotSubmultisetError : public MultisetError{
public:
    NotSubmultisetError():MultisetError("кратность элемента больше, чем в универсуме"){}
};

class CardinalityOverflowError : public MultisetError{
public:
    CardinalityOverflowError():MultisetError("мощность больше мощности ограничивающего мультимножества"){}
};


// мультимножество над носителем — функция кратности: counts[i] = k(x_i), x_i — i-й код Грея;
// универсум U и мультимножества A, B — объекты этого класса над общим носителем
class Multiset{
    std::shared_ptr<const CodeSet> carrier; // общий носитель
    std::vector<size_t> counts; // кратности

    // Сравнивает, одинаковый ли указатель у носителей
    void check_same_carrier(const Multiset& other) const; // - CarrierMismatchError
    // доп проверка на подмножество
    void check_within(const Multiset& universe) const; // - NotSubmultisetError

public:
    explicit Multiset(std::shared_ptr<const CodeSet> carrier); // все кратности равны нулю

    const CodeSet& get_carrier() const; // Ссылка на носитель
    size_t size() const; // число элементов носителя
    size_t get_count(size_t i) const; // получить кратность элемента
    void set_count(size_t i, size_t count);
    size_t cardinality() const; // сумма кратностей - мощность
    bool is_submultiset_of(const Multiset& other) const;

    // === операции ===

    //теор. мн.
    Multiset unite(const Multiset& b) const; // объединение
    Multiset intersect(const Multiset& b) const; // пересечение
    Multiset complement(const Multiset& universe) const; // дополнение
    Multiset difference(const Multiset& b, const Multiset& universe) const; // разность
    Multiset symmetric_difference(const Multiset& b, const Multiset& universe) const; // симметрическая разность

    //арифм.
    Multiset arithmetic_sum(const Multiset& b, const Multiset& universe) const; // сумма
    Multiset arithmetic_difference(const Multiset& b) const; // разность
    Multiset arithmetic_product(const Multiset& b, const Multiset& universe) const; // произведение
    Multiset arithmetic_division(const Multiset& b) const; // деление

    friend Multiset random_submultiset(const Multiset& bound, size_t cardinality); // автоматическое заполнение
};

// случайное подмультимножество bound мощности cardinality: cardinality раз случайный
// незаполненный элемент получает +1 к кратности
Multiset random_submultiset(const Multiset& bound, size_t cardinality);

// запись вида {000×2, 011×1}; элементы с нулевой кратностью не выводятся;
void print_multiset(std::ostream& out, const Multiset& multiset, size_t limit);
std::ostream& operator<<(std::ostream& out, const Multiset& multiset);
