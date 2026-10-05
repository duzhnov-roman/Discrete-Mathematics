#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

// наибольшая разрядность
const size_t MAX_DEPTH = 19;

// предел типа size_t
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

// Генерация кода Грея
std::vector<size_t> generate_gray_code(size_t depth); // при depth = 0 последовательность пуста

// двоичная запись кода длиной depth, старший разряд слева
std::string code_to_string(size_t code, size_t depth);

// носитель мультимножеств
class CodeSet{
    size_t depth;
    std::vector<size_t> codes;       // codes[i] — i-й код Грея
    std::vector<size_t> positions;   // positions[c] — номер кода c в последовательности

public:
    explicit CodeSet(size_t depth);

    size_t get_depth() const; // разрядность
    size_t size() const; // число элементов
    std::string get_code(size_t i) const;
    bool find(const std::string& code, size_t& index) const;
};
