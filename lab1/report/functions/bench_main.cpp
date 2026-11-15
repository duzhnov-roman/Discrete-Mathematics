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
