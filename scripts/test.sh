#!/bin/bash

rm -rf tmp
mkdir tmp
cd build
./ZpodsLib/test/zpods_lib_test
cd ..
#rm -rf tmp
