Multiset Multiset::unite(const Multiset& b) const{
    check_same_carrier(b);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = std::max(counts[i], b.counts[i]);
    }
    return res;
}
