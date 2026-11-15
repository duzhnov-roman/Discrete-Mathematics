bool CodeSet::find(const std::string& code, size_t& index) const{
    if(codes.empty() || code.size() != depth){
        return false;
    }
    size_t value = 0;
    for(size_t i = 0; i < code.size(); i++){
        if(code[i] != '0' && code[i] != '1') return false;
        value = value * 2 + (code[i] - '0');
    }
    index = positions[value];
    return true;
}
