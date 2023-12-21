#!/bin/bash

echo "***building server***"

if [ ! -d "scripts" ]; then
	echo "Please run this script from the root of the project"
	exit 1
fi

rm -rf client_image
mkdir client_image

cp Dockerfile.client client_image/Dockerfile

cp ./client.key ./client.crt ./server.crt ./client_image

cp ./build/network/client/cli/src/zpods_cli client_image/

cp /home/code4love/PROGRAM/cpp/Zpods/build/ZpodsLib/src/libzpods_lib.so client_image/libzpods_lib.so
cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30 client_image/libstdc++.so.6.0.30
cp /usr/lib/x86_64-linux-gnu/libm.so.6 client_image/libm.so.6
cp /usr/lib/x86_64-linux-gnu/libgcc_s.so.1 client_image/libgcc_s.so.1
cp /usr/lib/x86_64-linux-gnu/libc.so.6 client_image/libc.so.6
cp /usr/lib/x86_64-linux-gnu/libcrypto.so.3 client_image/libcrypto.so.3

cd client_image
docker build -t zpods_cli:latest .
cd ..

rm -rf client_image
