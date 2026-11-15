void Multiset::check_within(const Multiset& universe) const{
    if(!is_submultiset_of(universe)){
        throw NotSubmultisetError();
    }
}
