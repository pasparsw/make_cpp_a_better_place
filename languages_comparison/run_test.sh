#!/bin/bash

set -e

cd cpp && ./build_producer_consumer.sh && ./producer_consumer && ./build_lib_user.sh && ./lib_user
cd ..
cd cpp_front && ./build_producer_consumer.sh && ./producer_consumer && ./build_lib_user.sh && ./lib_user
