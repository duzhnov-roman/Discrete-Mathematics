Multiset Multiset::arithmetic_difference(const Multiset& b) const{
    check_same_carrier(b);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = (counts[i] > b.counts[i]) ? counts[i] - b.counts[i] : 0;
    }
    return res;
}
