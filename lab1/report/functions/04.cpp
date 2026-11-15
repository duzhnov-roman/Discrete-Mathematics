CodeSet::CodeSet(size_t depth):depth(depth),codes(generate_gray_code(depth)),positions(codes.size()){
    for(size_t i = 0; i < codes.size(); i++){
        positions[codes[i]] = i;
    }
}
