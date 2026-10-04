#include "core/GrayCode.hpp"


// функция Q из алгоритма: номер разряда (справа, с единицы), который меняется на шаге i,
// то есть число нулей в конце двоичной записи i плюс один
static size_t changed_bit(size_t i){
    size_t q = 1;
    size_t j = i;
    while(j % 2 == 0){
        j /= 2;
        q++;
    }
    return q;
}

std::vector<size_t> generate_gray_code(size_t depth){
    if(depth > MAX_TYPE_DEPTH){
        throw InvalidDepthError();
    }
    std::vector<size_t> codes;
    // слово длины 0 не содержит разрядов: в программе принято, что при n = 0 кодов нет
    if(depth == 0){
        return codes;
    }
    size_t count = size_t(1) << depth;
    codes.reserve(count);
    size_t code = 0;                            // шкала B = 00...0
    codes.push_back(code);
    for(size_t i = 1; i < count; i++){
        size_t p = changed_bit(i);              // номер разряда справа, 1 <= p <= depth
        code ^= size_t(1) << (p - 1);           // B[p] := 1 - B[p]
        codes.push_back(code);
    }
    return codes;
}

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

size_t CodeSet::get_depth() const{
    return depth;
}

size_t CodeSet::size() const{
    return codes.size();
}

std::string CodeSet::get_code(size_t i) const{
    if(i >= codes.size()){
        throw CodeIndexError();
    }
    return code_to_string(codes[i], depth);
}

bool CodeSet::find(const std::string& code, size_t& index) const{
    if(codes.empty() || code.size() != depth){
        return false;
    }
    size_t value = 0;
    for(size_t i = 0; i < code.size(); i++){
        if(code[i] != '0' && code[i] != '1') return false;
        value = value * 2 + (code[i] - '0');
    }
    // все 2^n слов длины n встречаются в коде Грея, поэтому номер есть у любого value
    index = positions[value];
    return true;
}
