static size_t changed_bit(size_t i){
    size_t q = 0;
    size_t j = i;
    while(j % 2 == 0){
        j /= 2;
        q++;
    }
    return q;
}
