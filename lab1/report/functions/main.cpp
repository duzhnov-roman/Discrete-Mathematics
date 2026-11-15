int main(int argc, char* argv[]){
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    unsigned seed = make_seed();
    if(argc == 3 && std::strcmp(argv[1], "--seed") == 0){
        seed = static_cast<unsigned>(std::strtoul(argv[2], nullptr, 10));
    }
    seed_random(seed);

    try{
        Menu menu;
        menu.run();
    }
    catch(const InputClosedError&){
        std::cout << std::endl << "ввод закончился" << std::endl;
    }
    catch(const std::exception& error){
        std::cout << "Внутренняя ошибка: " << error.what() << std::endl;
        return 1;
    }
    return 0;
}
