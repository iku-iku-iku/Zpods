#!/bin/bash

cd build
cmake ..
make
./ZpeeLib/test/ZpeeLib_test
cd ..
