Multiset Multiset::arithmetic_product(const Multiset& b, const Multiset& universe) const{
    check_within(universe);
    b.check_within(universe);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = std::min(counts[i] * b.counts[i], universe.counts[i]);
    }
    return res;
}
