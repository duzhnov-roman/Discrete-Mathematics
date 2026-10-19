#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>

class InputClosedError : public std::runtime_error{
public:
    InputClosedError():std::runtime_error("ввод закончился"){}
};

std::string read_text(const std::string& prompt);

bool parse_number(const std::string& text, size_t& value);

bool is_code(const std::string& text, size_t depth);

size_t read_number(const std::string& prompt, size_t min, size_t max);

bool read_number_or_empty(const std::string& prompt, size_t min, size_t max, size_t& value);

char read_letter(const std::string& prompt, const std::string& letters, bool allow_empty);
