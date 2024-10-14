#!/bin/bash
g++ cpp_functionality.cpp -O3 -DNDEBUG -march=native -std=c++23 -fPIC -shared -o libcppfunctionality.so
