#!/bin/bash

set -e

echo "Testing C++"
cd cpp && ./build_producer_consumer.sh && ./producer_consumer && ./build_lib_user.sh && ./lib_user
cd ..
echo "Testin C++ done"

echo "Testing CppFront"
cd cpp_front && ./build_producer_consumer.sh && ./producer_consumer && ./build_lib_user.sh && ./lib_user
cd ..
echo "Testing CppFront done"

echo "Testing D"
cd d && ./build_producer_consumer.sh && ./producer_consumer && ./build_lib_user.sh && ./lib_user
cd ..
echo "Testing D done"
