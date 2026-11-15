Multiset Multiset::difference(const Multiset& b, const Multiset& universe) const{
    return intersect(b.complement(universe));
}
