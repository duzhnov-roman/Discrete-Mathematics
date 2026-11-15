std::string code_to_string(size_t code, size_t depth){
    std::string res;
    for(size_t bit = depth; bit-- > 0;){
        res += ((code >> bit) & 1) ? '1' : '0';
    }
    return res;
}
