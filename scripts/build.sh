#!/bin/bash

mkdir -p build
cd build
cmake -G Ninja ..
cp compile_commands.json ../
ninja
cd ..
