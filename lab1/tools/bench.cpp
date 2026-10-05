// Замер времени самых долгих действий программы при разных разрядностях n.
// По нему выбрана MAX_DEPTH: наибольшее n, при котором любое действие занимает меньше 1 с.
// Сборка: g++ -std=c++14 -O2 -Isrc tools/bench.cpp src/core/*.cpp -o bench
#include "core/GrayCode.hpp"
#include "core/Multiset.hpp"
#include "core/Random.hpp"
#include "core/Universe.hpp"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>


// наименьшее из трёх измерений, мс: так меньше сказываются посторонние процессы
template<class Action>
static double measure(Action action){
    double best = 1e18;
    for(int run = 0; run < 3; run++){
        auto start = std::chrono::steady_clock::now();
        action();
        auto finish = std::chrono::steady_clock::now();
        best = std::min(best, std::chrono::duration<double, std::milli>(finish - start).count());
    }
    return best;
}

int main(){
    seed_random(1);
    std::cout << " n   код Грея   универсум   заполнение A   13 операций   наибольшее, мс" << std::endl;
    for(size_t depth = 10; depth <= MAX_DEPTH + 2; depth++){
        std::shared_ptr<const CodeSet> carrier;
        double t_code = measure([&]{ carrier = std::make_shared<CodeSet>(depth); });

        // наихудший случай: все кратности равны MAX_MULTIPLICITY
        Multiset universe(carrier);
        double t_universe = measure([&]{ universe = uniform_universe(carrier, MAX_MULTIPLICITY); });

        // заполнение делает |A| шагов, поэтому дольше всего при |A| = |U|
        Multiset a(carrier);
        Multiset b(carrier);
        double t_fill = measure([&]{ a = random_submultiset(universe, universe.cardinality()); });
        a = random_submultiset(universe, universe.cardinality() / 2);
        b = random_submultiset(universe, universe.cardinality() / 3);

        size_t checksum = 0;
        double t_operations = measure([&]{
            checksum += a.unite(b).cardinality() + a.intersect(b).cardinality()
                      + a.difference(b, universe).cardinality() + b.difference(a, universe).cardinality()
                      + a.symmetric_difference(b, universe).cardinality()
                      + a.complement(universe).cardinality() + b.complement(universe).cardinality()
                      + a.arithmetic_sum(b, universe).cardinality() + a.arithmetic_difference(b).cardinality()
                      + b.arithmetic_difference(a).cardinality() + a.arithmetic_product(b, universe).cardinality()
                      + a.arithmetic_division(b).cardinality() + b.arithmetic_division(a).cardinality();
        });

        double worst = std::max({t_code, t_universe, t_fill, t_operations});
        std::cout << std::fixed << std::setprecision(1) << std::setw(2) << depth
                  << std::setw(11) << t_code << std::setw(12) << t_universe << std::setw(15) << t_fill
                  << std::setw(14) << t_operations << std::setw(17) << worst
                  << (worst < 1000 ? "" : "  > 1 с") << std::endl;
        if(checksum == 0) std::cout << "";   // не даёт компилятору выбросить операции
    }
    return 0;
}
