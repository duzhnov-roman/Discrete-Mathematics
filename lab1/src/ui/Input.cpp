#include "ui/Input.hpp"
#include <iostream>
#ifdef _WIN32
#include <io.h>
static bool stdin_is_console(){ return _isatty(0) != 0; }
#else
#include <unistd.h>
static bool stdin_is_console(){ return isatty(0) != 0; }
#endif


std::string read_text(const std::string& prompt){
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

// не больше 9 цифр: число до 999 999 999 гарантированно помещается в size_t
bool parse_number(const std::string& text, size_t& value){
    if(text.empty() || text.size() > 9) return false;
    size_t res = 0;
    for(size_t i = 0; i < text.size(); i++){
        if(text[i] < '0' || text[i] > '9') return false;
        res = res * 10 + (text[i] - '0');
    }
    value = res;
    return true;
}

bool is_code(const std::string& text, size_t depth){
    if(text.size() != depth) return false;
    for(size_t i = 0; i < text.size(); i++){
        if(text[i] != '0' && text[i] != '1') return false;
    }
    return true;
}

// сообщение называет причину: пустая строка, не число или число вне отрезка
static void report_bad_number(const std::string& text, size_t min, size_t max){
    std::string range = "[" + std::to_string(min) + ".." + std::to_string(max) + "]";
    size_t value = 0;
    if(text.empty()){
        std::cout << "  ! пустая строка: нужно целое число из " << range << std::endl;
    }
    else if(parse_number(text, value)){
        std::cout << "  ! " << value << " не входит в " << range << std::endl;
    }
    else{
        std::cout << "  ! «" << text << "» не целое неотрицательное число; нужно число из " << range << std::endl;
    }
}

size_t read_number(const std::string& prompt, size_t min, size_t max){
    while(true){
        std::string line = read_text(prompt);
        size_t value = 0;
        if(parse_number(line, value) && value >= min && value <= max){
            return value;
        }
        report_bad_number(line, min, max);
    }
}

bool read_number_or_empty(const std::string& prompt, size_t min, size_t max, size_t& value){
    while(true){
        std::string line = read_text(prompt);
        if(line.empty()) return false;
        size_t res = 0;
        if(parse_number(line, res) && res >= min && res <= max){
            value = res;
            return true;
        }
        report_bad_number(line, min, max);
    }
}

char read_letter(const std::string& prompt, const std::string& letters, bool allow_empty){
    std::string listed;
    for(size_t i = 0; i < letters.size(); i++){
        listed += (i > 0 ? " " : "") + std::string(1, letters[i]);
    }
    while(true){
        std::string line = read_text(prompt);
        if(line.empty() && allow_empty) return '\0';
        if(line.size() == 1){
            char letter = line[0];
            if(letter >= 'A' && letter <= 'Z') letter = letter - 'A' + 'a';
            if(letters.find(letter) != std::string::npos) return letter;
        }
        if(line.empty()){
            std::cout << "  ! пустая строка: введите одну из букв " << listed << std::endl;
        }
        else{
            std::cout << "  ! «" << line << "» не команда; допустимы буквы " << listed << std::endl;
        }
    }
}
