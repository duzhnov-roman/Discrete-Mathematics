#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>

// ввод закончился (Ctrl+Z в Windows, Ctrl+D в Linux, конец файла)
class InputClosedError : public std::runtime_error{
public:
    InputClosedError():std::runtime_error("ввод закончился"){}
};

// печатает приглашение и читает строку без пробелов, табуляций и \r по краям
std::string read_text(const std::string& prompt);

// строка из 1..9 цифр; число записывается в value
bool parse_number(const std::string& text, size_t& value);

// строка длины depth из символов 0 и 1
bool is_code(const std::string& text, size_t depth);

// переспрашивает, пока не будет введено целое число от min до max
size_t read_number(const std::string& prompt, size_t min, size_t max);

// то же, но пустая строка допустима: тогда возвращается false, value не меняется
bool read_number_or_empty(const std::string& prompt, size_t min, size_t max, size_t& value);

// переспрашивает, пока не будет введена одна буква из letters (регистр не важен);
// если allow_empty, пустая строка допустима и возвращается '\0'
char read_letter(const std::string& prompt, const std::string& letters, bool allow_empty);
