#include "core/GrayCode.hpp"
#include "core/Universe.hpp"
#include "core/Random.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
int main(){
    for(size_t n=0; n<=MAX_DEPTH; ++n){
        CodeSet codes(n);
        assert(codes.size()==(size_t(1)<<n));
        auto values=generate_gray_code(n);
        for(size_t i=0; i<values.size(); ++i){
            assert(values[i]==(i^(i>>1)));
            size_t found=codes.size();
            assert(codes.find(codes.get_code(i),found) && found==i);
            if(i){size_t diff=values[i]^values[i-1]; assert(diff && !(diff&(diff-1)));}
        }
    }
    auto carrier=std::make_shared<CodeSet>(0);
    auto universe=uniform_universe(carrier,3);
    seed_random(11);
    for(size_t m=0;m<=3;++m){
        auto a=random_submultiset(universe,m);
        assert(a.cardinality()==m && a.get_count(0)==m);
        assert(a.complement(universe).get_count(0)==3-m);
    }
    std::ostringstream text;
    text<<universe;
    assert(text.str()=="⟨3(ε)⟩");
    std::cout<<"Gray code n=0..19, reverse lookup, zero-depth filling and formatting: PASS\n";
}
