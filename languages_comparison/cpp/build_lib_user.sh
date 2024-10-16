#!/bin/bash

g++ lib_user.cpp cpp_functionality.cpp -O3 -DNDEBUG -march=native -std=c++23 -o lib_user
