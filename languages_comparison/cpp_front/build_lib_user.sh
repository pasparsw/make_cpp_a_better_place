#!/bin/bash

./cppfront lib_user.cpp2 -im &&
g++ lib_user.cpp cpp_functionality.cpp -I cpp_front_headers -std=c++23 -o lib_user
