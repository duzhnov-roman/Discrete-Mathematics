bool Multiset::is_submultiset_of(const Multiset& other) const{
    check_same_carrier(other);
    for(size_t i = 0; i < counts.size(); i++){
        if(counts[i] > other.counts[i]) return false;
    }
    return true;
}
