Multiset Multiset::symmetric_difference(const Multiset& b, const Multiset& universe) const{
    return difference(b, universe).unite(b.difference(*this, universe));
}
