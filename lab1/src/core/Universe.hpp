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


// универсум, в котором каждый код Грея входит с одной и той же кратностью multiplicity
Multiset uniform_universe(std::shared_ptr<const CodeSet> carrier, size_t multiplicity);
