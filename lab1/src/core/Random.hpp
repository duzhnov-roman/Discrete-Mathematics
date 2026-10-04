#pragma once
#include <cstddef>

// зерно для генератора: смесь std::random_device и текущего времени
unsigned make_seed();

// задаёт начальное состояние генератора; одно зерно — одна и та же последовательность чисел
void seed_random(unsigned seed);

// равновероятное целое число от 0 до bound - 1; bound должно быть больше нуля
size_t random_below(size_t bound);
