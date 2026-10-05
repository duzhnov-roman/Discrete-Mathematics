#include "core/Universe.hpp"


Multiset uniform_universe(std::shared_ptr<const CodeSet> carrier, size_t multiplicity){
    if(multiplicity < 1 || multiplicity > MAX_MULTIPLICITY){
        throw InvalidMultiplicityError();
    }
    Multiset universe(carrier);
    for(size_t i = 0; i < universe.size(); i++){
        universe.set_count(i, multiplicity);
    }
    return universe;
}
