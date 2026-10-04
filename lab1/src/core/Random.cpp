#include "core/Random.hpp"
#include <chrono>
#include <random>


// вихрь Мерсенна: период 2^19937 - 1, значения 32-битные (у std::rand всего 15 бит)
static std::mt19937& generator(){
    static std::mt19937 gen;
    return gen;
}

unsigned make_seed(){
    // в MinGW std::random_device детерминирован и при каждом запуске выдаёт
    // одну и ту же последовательность, поэтому к нему примешивается время
    std::random_device device;
    unsigned time = static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    return device() ^ time;
}

void seed_random(unsigned seed){
    generator().seed(seed);
}

size_t random_below(size_t bound){
    std::uniform_int_distribution<size_t> distribution(0, bound - 1);
    return distribution(generator());
}
