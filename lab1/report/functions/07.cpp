std::string CodeSet::get_code(size_t i) const{
    if(i >= codes.size()){
        throw CodeIndexError();
    }
    return code_to_string(codes[i], depth);
}
