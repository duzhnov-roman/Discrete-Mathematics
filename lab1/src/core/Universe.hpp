#pragma once
#include "core/GrayCode.hpp"
#include "core/Multiset.hpp"
#include <cstddef>
#include <memory>

// наибольшая кратность элемента универсума
const size_t MAX_MULTIPLICITY = 100;


class InvalidMultiplicityError : public MultisetError{
public:
    InvalidMultiplicityError():MultisetError("кратность элемента универсума вне диапазона от 1 до MAX_MULTIPLICITY"){}
};

class UniverseCardinalityError : public MultisetError{
public:
    UniverseCardinalityError():MultisetError("мощность универсума вне диапазона от N до N * MAX_MULTIPLICITY"){}
};


// наименьшая и наибольшая мощность универсума над носителем из size элементов
size_t min_universe_cardinality(size_t size);
size_t max_universe_cardinality(size_t size);

// универсум, в котором кратность каждого элемента — случайное число от 1 до max_multiplicity
Multiset random_universe(std::shared_ptr<const CodeSet> carrier, size_t max_multiplicity);

// универсум заданной мощности: каждый элемент получает кратность 1,
// остальные cardinality - N единиц распределяются случайно с ограничением MAX_MULTIPLICITY
Multiset universe_with_cardinality(std::shared_ptr<const CodeSet> carrier, size_t cardinality);
