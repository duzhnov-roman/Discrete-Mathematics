#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

// наибольшая разрядность, которую предлагает интерфейс; обоснование — tools/bench.cpp и отчёт, п. 2.2
const size_t MAX_DEPTH = 19;

// предел типа: 2^depth должно помещаться в size_t
const size_t MAX_TYPE_DEPTH = sizeof(size_t) * 8 - 1;


class GrayCodeError : public std::runtime_error{
public:
    explicit GrayCodeError(const std::string& message):std::runtime_error(message){}
};

class InvalidDepthError : public GrayCodeError{
public:
    InvalidDepthError():GrayCodeError("2^n не помещается в size_t"){}
};

class CodeIndexError : public GrayCodeError{
public:
    CodeIndexError():GrayCodeError("номер кода вне последовательности"){}
};


// коды Грея разрядности depth в порядке построения алгоритмом из [Новиков] (функция Q);
// при depth = 0 последовательность пуста
std::vector<size_t> generate_gray_code(size_t depth);

// двоичная запись кода длиной depth, старший разряд слева
std::string code_to_string(size_t code, size_t depth);


// носитель мультимножеств: все 2^n кодов Грея разрядности n
class CodeSet{
    size_t depth;
    std::vector<size_t> codes;       // codes[i] — i-й код Грея
    std::vector<size_t> positions;   // positions[c] — номер кода c в последовательности

public:
    explicit CodeSet(size_t depth);

    size_t get_depth() const;
    size_t size() const;
    std::string get_code(size_t i) const;
    bool find(const std::string& code, size_t& index) const;
};
