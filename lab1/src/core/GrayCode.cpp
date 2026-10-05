#include "core/GrayCode.hpp"


// Функция, определяющая, какой бит перевернуть
static size_t changed_bit(size_t i){
    size_t q = 0;
    size_t j = i;
    while(j % 2 == 0){
        j /= 2;
        q++;
    }
    // число нулей в конце двоичной записи i — номер бита справа, считая с нуля
    return q;
}

std::vector<size_t> generate_gray_code(size_t depth){
    /*
        Алгоритм начинает с 0 и на каждом шаге переворачивает
        один бит, какой бит перевернуть - решает функция changed_bit()
    */
    if(depth > MAX_TYPE_DEPTH){
        throw InvalidDepthError();
    }
    std::vector<size_t> codes;
    // при n = 0 кодов нет
    if(depth == 0){
        return codes;
    }
    size_t count = size_t(1) << depth;
    codes.reserve(count);
    size_t code = 0;
    codes.push_back(code);
    for(size_t i = 1; i < count; i++){
        size_t p = changed_bit(i);
        code ^= size_t(1) << p;
        codes.push_back(code);
    }
    return codes;
}

// перевод кода Грея в строку
std::string code_to_string(size_t code, size_t depth){
    std::string res;
    for(size_t bit = depth; bit-- > 0;){
        res += ((code >> bit) & 1) ? '1' : '0';
    }
    return res;
}


CodeSet::CodeSet(size_t depth):depth(depth),codes(generate_gray_code(depth)),positions(codes.size()){
    for(size_t i = 0; i < codes.size(); i++){
        positions[codes[i]] = i;
    }
}

// получить длину слова
size_t CodeSet::get_depth() const{
    return depth;
}

// получить размер вектора
size_t CodeSet::size() const{
    return codes.size();
}

// получить код по индексу
std::string CodeSet::get_code(size_t i) const{
    if(i >= codes.size()){
        throw CodeIndexError();
    }
    return code_to_string(codes[i], depth);
}

// получить индекс по коду
bool CodeSet::find(const std::string& code, size_t& index) const{
    if(codes.empty() || code.size() != depth){
        return false;
    }
    size_t value = 0;
    for(size_t i = 0; i < code.size(); i++){
        if(code[i] != '0' && code[i] != '1') return false;
        value = value * 2 + (code[i] - '0'); // превращает строку в число
    }
    // все 2^n слов длины n встречаются в коде Грея, поэтому номер есть у любого value
    index = positions[value];
    return true;
}
