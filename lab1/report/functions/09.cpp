static std::mt19937& generator(){
    static std::mt19937 gen;
    return gen;
}
