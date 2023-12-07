#!/bin/bash

sudo apt-get install libzstd-dev liblz4-dev libbz2-dev zlib1g-dev libsnappy-dev libgflags-dev -y
cd extern/rocksdb
make shared_lib
sudo make install-shared PREFIX=/usr/local/rocksdb
