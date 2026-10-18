#pragma once
#include <cstddef>

unsigned make_seed();

void seed_random(unsigned seed);

size_t random_below(size_t bound);
