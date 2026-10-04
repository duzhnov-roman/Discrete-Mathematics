#include "ui/Input.hpp"
#include <iostream>
#ifdef _WIN32
#include <io.h>
static bool stdin_is_console(){ return _isatty(0) != 0; }
#else
#include <unistd.h>
static bool stdin_is_console(){ return isatty(0) != 0; }
#endif


// печатает приглашение и читает строку без пробелов, табуляций и \r по краям
static std::string read_line(const std::string& prompt){
    std::cout << prompt;
    std::string line;
    if(!std::getline(std::cin, line)){
        throw InputClosedError();
    }
    // при вводе из файла строка не отображается консолью, поэтому программа выводит её сама
    static const bool echo = !stdin_is_console();
    if(echo) std::cout << line.substr(0, line.find('\r')) << std::endl;
    size_t begin = line.find_first_not_of(" \t\r");
    if(begin == std::string::npos) return "";
    size_t end = line.find_last_not_of(" \t\r");
    return line.substr(begin, end - begin + 1);
}

// строка из 1..9 цифр: число не больше 999 999 999 гарантированно помещается в size_t
static bool parse_number(const std::string& text, size_t& value){
    if(text.empty() || text.size() > 9) return false;
    size_t res = 0;
    for(size_t i = 0; i < text.size(); i++){
        if(text[i] < '0' || text[i] > '9') return false;
        res = res * 10 + (text[i] - '0');
    }
    value = res;
    return true;
}

// строка длины depth из символов 0 и 1
static bool is_code(const std::string& text, size_t depth){
    if(text.size() != depth) return false;
    for(size_t i = 0; i < text.size(); i++){
        if(text[i] != '0' && text[i] != '1') return false;
    }
    return true;
}

static void print_range_error(size_t min, size_t max){
    std::cout << "  Ошибка: введите целое число от " << min << " до " << max << std::endl;
}

size_t read_number(const std::string& prompt, size_t min, size_t max){
    while(true){
        size_t value = 0;
        if(parse_number(read_line(prompt), value) && value >= min && value <= max){
            return value;
        }
        print_range_error(min, max);
    }
}

bool read_number_or_empty(const std::string& prompt, size_t min, size_t max, size_t& value){
    while(true){
        std::string line = read_line(prompt);
        if(line.empty()) return false;
        size_t res = 0;
        if(parse_number(line, res) && res >= min && res <= max){
            value = res;
            return true;
        }
        print_range_error(min, max);
    }
}

bool read_code_or_empty(const std::string& prompt, size_t depth, std::string& code){
    while(true){
        std::string line = read_line(prompt);
        if(line.empty()) return false;
        if(is_code(line, depth)){
            code = line;
            return true;
        }
        std::cout << "  Ошибка: код должен состоять из " << depth << " символов 0 и 1" << std::endl;
    }
}
