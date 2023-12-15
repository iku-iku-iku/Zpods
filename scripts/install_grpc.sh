#!/bin/bash

# make sure directory extern/grpc exists
if [ ! -d "extern/grpc" ]; then
	echo "Please run this script from the root directory of the project"
	exit 1
fi

cd extern/grpc
git submodule update --init --recursive
mkdir -p cmake/build
cd cmake/build
cmake ../..
make -j8
make install
