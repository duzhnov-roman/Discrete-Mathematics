size_t Multiset::cardinality() const{
    size_t res = 0;
    for(size_t i = 0; i < counts.size(); i++){
        res += counts[i];
    }
    return res;
}
