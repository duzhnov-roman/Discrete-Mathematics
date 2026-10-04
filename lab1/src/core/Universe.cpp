#include "core/Universe.hpp"
#include "core/Random.hpp"


size_t min_universe_cardinality(size_t size){
    return size;
}

size_t max_universe_cardinality(size_t size){
    return size * MAX_MULTIPLICITY;
}

Multiset random_universe(std::shared_ptr<const CodeSet> carrier, size_t max_multiplicity){
    if(max_multiplicity < 1 || max_multiplicity > MAX_MULTIPLICITY){
        throw InvalidMultiplicityError();
    }
    Multiset universe(carrier);
    for(size_t i = 0; i < universe.size(); i++){
        universe.set_count(i, 1 + random_below(max_multiplicity));
    }
    return universe;
}

Multiset universe_with_cardinality(std::shared_ptr<const CodeSet> carrier, size_t cardinality){
    size_t size = carrier->size();
    if(cardinality < min_universe_cardinality(size) || cardinality > max_universe_cardinality(size)){
        throw UniverseCardinalityError();
    }
    // сверх обязательной единицы у каждого элемента есть MAX_MULTIPLICITY - 1 свободных мест
    Multiset extra(carrier);
    for(size_t i = 0; i < size; i++){
        extra.set_count(i, MAX_MULTIPLICITY - 1);
    }
    Multiset universe = random_submultiset(extra, cardinality - size);
    for(size_t i = 0; i < size; i++){
        universe.set_count(i, universe.get_count(i) + 1);
    }
    return universe;
}
