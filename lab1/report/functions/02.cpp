std::vector<size_t> generate_gray_code(size_t depth){
    if(depth > MAX_TYPE_DEPTH){
        throw InvalidDepthError();
    }
    std::vector<size_t> codes;
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
