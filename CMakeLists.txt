project (magic_square)
cmake_minimum_required (VERSION 2.8)

add_definitions (-std=c++11 -Wall -Ofast -Wl --no-as-needed)

add_executable(run main.cpp WorkerThread.hpp)
target_link_libraries(run pthread rt)
