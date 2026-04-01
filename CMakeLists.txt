cmake_minimum_required(VERSION 3.10)
project(Mic1)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    src
    src/ula
    src/reg
    src/lexer
    src/auxiliarFunctions
)

add_executable(mic1
    src/Mic1.cpp
    src/ula/ULA.cpp
    src/reg/register.cpp
    src/lexer/lexer.cpp
    src/auxiliarFunctions/functions.cpp
)