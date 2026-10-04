cmake_minimum_required(VERSION 3.10)
project(lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES
    src/core/GrayCode.cpp
    src/core/Random.cpp
    src/core/Multiset.cpp
    src/core/Universe.cpp
)

add_executable(lab1 src/main.cpp src/ui/Input.cpp src/ui/Menu.cpp ${CORE_SOURCES})
target_include_directories(lab1 PRIVATE src)

# замер времени, по которому выбрана MAX_DEPTH
add_executable(bench tools/bench.cpp ${CORE_SOURCES})
target_include_directories(bench PRIVATE src)
