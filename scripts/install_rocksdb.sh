#!/bin/bash

# make sure extern/rocksdb exists

if [ ! -d "extern/rocksdb" ]; then
	echo "Please run this script from the root directory of the project"
	exit 1
fi

sudo apt-get install libzstd-dev liblz4-dev libbz2-dev zlib1g-dev libsnappy-dev libgflags-dev -y
cd extern/rocksdb
make -j8
sudo make install
