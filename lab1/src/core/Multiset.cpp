#include "core/Multiset.hpp"
#include "core/Random.hpp"
#include <algorithm>


Multiset::Multiset(std::shared_ptr<const CodeSet> carrier):carrier(carrier),counts(carrier->size(), 0){}

void Multiset::check_same_carrier(const Multiset& other) const{
    if(carrier != other.carrier){
        throw CarrierMismatchError();
    }
}

void Multiset::check_within(const Multiset& universe) const{
    check_same_carrier(universe);
    if(!is_submultiset_of(universe)){
        throw NotSubmultisetError();
    }
}

const CodeSet& Multiset::get_carrier() const{
    return *carrier;
}

size_t Multiset::size() const{
    return counts.size();
}

size_t Multiset::get_count(size_t i) const{
    if(i >= counts.size()){
        throw MultisetIndexError();
    }
    return counts[i];
}

void Multiset::set_count(size_t i, size_t count){
    if(i >= counts.size()){
        throw MultisetIndexError();
    }
    counts[i] = count;
}

size_t Multiset::cardinality() const{
    size_t res = 0;
    for(size_t i = 0; i < counts.size(); i++){
        res += counts[i];
    }
    return res;
}

bool Multiset::is_submultiset_of(const Multiset& other) const{
    check_same_carrier(other);
    for(size_t i = 0; i < counts.size(); i++){
        if(counts[i] > other.counts[i]) return false;
    }
    return true;
}

Multiset Multiset::unite(const Multiset& b) const{
    check_same_carrier(b);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = std::max(counts[i], b.counts[i]);
    }
    return res;
}

Multiset Multiset::intersect(const Multiset& b) const{
    check_same_carrier(b);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = std::min(counts[i], b.counts[i]);
    }
    return res;
}

Multiset Multiset::complement(const Multiset& universe) const{
    check_within(universe);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = universe.counts[i] - counts[i];   // >= 0, так как A ⊆ U
    }
    return res;
}

Multiset Multiset::difference(const Multiset& b, const Multiset& universe) const{
    // A \ B = A ∩ ¬B
    return intersect(b.complement(universe));
}

Multiset Multiset::symmetric_difference(const Multiset& b, const Multiset& universe) const{
    // A ∆ B = (A \ B) ∪ (B \ A)
    return difference(b, universe).unite(b.difference(*this, universe));
}

Multiset Multiset::arithmetic_sum(const Multiset& b, const Multiset& universe) const{
    check_within(universe);
    b.check_within(universe);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = std::min(counts[i] + b.counts[i], universe.counts[i]);
    }
    return res;
}

Multiset Multiset::arithmetic_difference(const Multiset& b) const{
    check_same_carrier(b);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = (counts[i] > b.counts[i]) ? counts[i] - b.counts[i] : 0;
    }
    return res;
}

Multiset Multiset::arithmetic_product(const Multiset& b, const Multiset& universe) const{
    check_within(universe);
    b.check_within(universe);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = std::min(counts[i] * b.counts[i], universe.counts[i]);
    }
    return res;
}

Multiset Multiset::arithmetic_division(const Multiset& b) const{
    check_same_carrier(b);
    Multiset res(carrier);
    for(size_t i = 0; i < counts.size(); i++){
        res.counts[i] = (b.counts[i] != 0) ? counts[i] / b.counts[i] : 0;
    }
    return res;
}


Multiset random_submultiset(const Multiset& bound, size_t cardinality){
    size_t left = bound.cardinality();   // единиц, которые ещё не просмотрены
    if(cardinality > left){
        throw CardinalityOverflowError();
    }
    // элемент x_i — это bound[i] одинаковых шаров в урне; единицы просматриваются по очереди,
    // и каждая берётся с вероятностью need / left (алгоритм S, Кнут, т. 2, п. 3.4.2)
    Multiset res(bound.carrier);
    size_t need = cardinality;
    for(size_t i = 0; i < bound.counts.size() && need > 0; i++){
        size_t taken = 0;
        for(size_t unit = 0; unit < bound.counts[i] && need > 0; unit++){
            if(need == left || random_below(left) < need){
                taken++;
                need--;
            }
            left--;
        }
        res.counts[i] = taken;
    }
    return res;
}


void print_multiset(std::ostream& out, const Multiset& multiset, size_t limit){
    const CodeSet& carrier = multiset.get_carrier();
    size_t shown = 0;
    size_t hidden = 0;
    out << "{";
    for(size_t i = 0; i < multiset.size(); i++){
        size_t count = multiset.get_count(i);
        if(count == 0) continue;
        if(shown == limit){
            hidden++;
            continue;
        }
        if(shown > 0) out << ", ";
        out << carrier.get_code(i) << "(" << count << ")";
        shown++;
    }
    if(hidden > 0) out << ", ... ещё " << hidden << " эл.";
    out << "}";
}

std::ostream& operator<<(std::ostream& out, const Multiset& multiset){
    print_multiset(out, multiset, multiset.size());
    return out;
}
