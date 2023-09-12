#!/bin/bash

mkdir -p build
cd build
cmake ..
make
./ZpodsLib/test/ZpodsLib_test
cd ..
