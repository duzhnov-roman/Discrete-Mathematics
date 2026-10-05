#include "core/Random.hpp"
#include "ui/Input.hpp"
#include "ui/Menu.hpp"
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif


int main(int argc, char* argv[]){
#ifdef _WIN32
    // строки программы записаны в UTF-8, а консоль Windows по умолчанию выводит в кодировке 866
    SetConsoleOutputCP(CP_UTF8);
#endif
    // lab1 --seed N повторяет запуск с теми же случайными числами
    unsigned seed = make_seed();
    if(argc == 3 && std::strcmp(argv[1], "--seed") == 0){
        seed = static_cast<unsigned>(std::strtoul(argv[2], nullptr, 10));
    }
    seed_random(seed);
    std::cout << "seed = " << seed << std::endl;

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
