Multiset random_submultiset(const Multiset& bound, size_t cardinality){
    if(cardinality > bound.cardinality()){
        throw CardinalityOverflowError();
    }
    Multiset res(bound.carrier);
    std::vector<size_t> free;
    free.reserve(bound.counts.size());
    for(size_t i = 0; i < bound.counts.size(); i++){
        if(bound.counts[i] > 0) free.push_back(i);
    }
    for(size_t step = 0; step < cardinality; step++){
        size_t j = random_below(free.size());
        size_t i = free[j];
        res.counts[i]++;
        if(res.counts[i] == bound.counts[i]){
            free[j] = free.back();
            free.pop_back();
        }
    }
    return res;
}
