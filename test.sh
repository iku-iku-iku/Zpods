#!/bin/bash

mkdir -p build
cd build
cmake ..
make
./ZpeeLib/test/ZpeeLib_test
cd ..
