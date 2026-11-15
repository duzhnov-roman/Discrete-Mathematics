void Multiset::check_same_carrier(const Multiset& other) const{
    if(carrier != other.carrier){
        throw CarrierMismatchError();
    }
}
