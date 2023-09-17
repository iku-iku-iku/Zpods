#!/bin/bash

mkdir -p build
cd build
cmake ..
make
./ZpodsLib/test/zpods_lib_test
cd ..
