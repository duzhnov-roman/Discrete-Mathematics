Multiset Multiset::arithmetic_division(const Multiset& b) const{
    check_same_carrier(b);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = (b.counts[i] != 0) ? counts[i] / b.counts[i] : 0;
    }
    return res;
}
