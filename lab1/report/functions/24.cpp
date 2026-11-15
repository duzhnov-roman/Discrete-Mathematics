Multiset Multiset::complement(const Multiset& universe) const{
    check_within(universe);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = universe.counts[i] - counts[i];
    }
    return res;
}
