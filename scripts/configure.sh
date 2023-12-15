#!/bin/bash
git submodule update --init --recursive
sudo apt install ceph librados2 librados-dev -y
sudo apt install libssl3 libssl-dev -y
