#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>

// ввод закончился (Ctrl+Z в Windows, Ctrl+D в Linux, конец файла)
class InputClosedError : public std::runtime_error{
public:
    InputClosedError():std::runtime_error("ввод закончился"){}
};

// переспрашивает, пока не будет введено целое число от min до max
size_t read_number(const std::string& prompt, size_t min, size_t max);

// то же, но пустая строка допустима: тогда возвращается false, value не меняется
bool read_number_or_empty(const std::string& prompt, size_t min, size_t max, size_t& value);

// переспрашивает, пока не будет введён код длины depth из символов 0 и 1;
// пустая строка — отказ от ввода, тогда возвращается false
bool read_code_or_empty(const std::string& prompt, size_t depth, std::string& code);
