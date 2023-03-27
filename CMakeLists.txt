cmake_minimum_required(VERSION 3.23)
project(automidterm)

set(CMAKE_CXX_STANDARD 14)

add_executable(automidterm main.cpp NFA.cpp NFA.h DFA.cpp DFA.h MinDFA.cpp MinDFA.h)
