#!/bin/bash

g++ -c cpp_functionality.cpp &&
dmd lib_user.d cpp_functionality.o
