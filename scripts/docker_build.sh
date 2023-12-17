#!/bin/bash

rm -rf server_image
mkdir server_image

cp server.key *.crt server_image/

echo "***building server***"

if [ ! -d "scripts" ]; then
	echo "Please run this script from the root of the project"
	exit 1
fi

cp Dockerfile.server server_image/Dockerfile

cp build/network/server/src/zpods_server server_image/

cp build/ZpodsLib/src/libzpods_lib.so server_image/
cp /usr/lib/x86_64-linux-gnu/libzstd.so.1.4.8 server_image/libzstd.so.1.4.8
cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30 server_image/libstdc++.so.6.0.30
cp /usr/lib/x86_64-linux-gnu/libm.so.6 server_image/libm.so.6
cp /usr/lib/x86_64-linux-gnu/libgcc_s.so.1 server_image/libgcc_s.so.1
cp /usr/lib/x86_64-linux-gnu/libc.so.6 server_image/libc.so.6

cd server_image

docker build -t zpods_server:latest .

# clean
cd ..
rm server_image/libzstd.so.1.4.8
rm server_image/libstdc++.so.6.0.30
rm server_image/libm.so.6
rm server_image/libgcc_s.so.1
rm server_image/libc.so.6
