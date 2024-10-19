#!/bin/bash

./cppfront producer_consumer.cpp2 -im &&
g++ producer_consumer.cpp -I cpp_front_headers -O3 -DNDEBUG -march=native -std=c++23 -o producer_consumer
