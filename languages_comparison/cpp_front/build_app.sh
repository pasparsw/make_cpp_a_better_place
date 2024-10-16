#!/bin/bash

# Note: here cppfront is an aliased path to where cppfront executable is located
./cppfront main.cpp2 &&
g++ main.cpp -I cpp_front_headers -O3 -DNDEBUG -march=native -std=c++23 -o app
