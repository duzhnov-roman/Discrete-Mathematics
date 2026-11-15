#include "core/GrayCode.hpp"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

static double seconds(){
#ifdef _WIN32
    static const double frequency = []{
        LARGE_INTEGER value;
        QueryPerformanceFrequency(&value);
        return static_cast<double>(value.QuadPart);
    }();
    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    return static_cast<double>(value.QuadPart) / frequency;
#else
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
#endif
}

int main(){
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    const size_t runs = 21;
    std::cout << "Формирование универсума U; медиана " << runs << " измерения после прогрева" << std::endl;
    std::cout << " n   время формирования, мс" << std::endl;
    for(size_t depth = 10; depth <= MAX_DEPTH; depth++){
        std::vector<double> samples;
        for(size_t run = 0; run <= runs; run++){
            double start = seconds();
            auto carrier = std::make_shared<CodeSet>(depth);
            double finish = seconds();
            if(carrier->size() != (size_t(1) << depth)) return 1;
            if(run > 0) samples.push_back((finish - start) * 1000.0);
        }
        std::sort(samples.begin(), samples.end());
        std::cout << std::setw(2) << depth << std::fixed << std::setprecision(4)
                  << std::setw(25) << samples[runs / 2] << std::endl;
    }
}
