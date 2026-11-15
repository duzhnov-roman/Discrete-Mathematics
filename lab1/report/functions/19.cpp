void Multiset::set_count(size_t i, size_t count){
    if(i >= counts.size()){
        throw MultisetIndexError();
    }
    counts[i] = count;
}
