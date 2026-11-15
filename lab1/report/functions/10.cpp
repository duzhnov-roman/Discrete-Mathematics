unsigned make_seed(){
    std::random_device device;
    unsigned time = static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    return device() ^ time;
}
