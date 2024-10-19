#!/bin/bash

g++ producer_consumer.cpp -O3 -DNDEBUG -march=native -std=c++23 -o producer_consumer
