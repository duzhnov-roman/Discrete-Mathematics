size_t Multiset::get_count(size_t i) const{
    if(i >= counts.size()){
        throw MultisetIndexError();
    }
    return counts[i];
}
