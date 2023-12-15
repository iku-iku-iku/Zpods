#!/bin/bash

if [ ! -d "scripts" ]; then
	echo "Please run this script from the root of the project"
	exit 1
fi

rm -rf server_image
mkdir server_image
cp Dockerfile.server server_image/Dockerfile

cp build/network/server/src/zpods_server server_image/
cp build/ZpodsLib/src/libzpods_lib.so server_image/

cp /usr/local/lib/librocksdb.so.8.10.0 server_image/librocksdb.so.8.10.0
cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30 server_image/libstdc++.so.6.0.30
cp /usr/lib/x86_64-linux-gnu/libm.so.6 server_image/libm.so.6
cp /usr/lib/x86_64-linux-gnu/libgcc_s.so.1 server_image/libgcc_s.so.1
cp /usr/lib/x86_64-linux-gnu/libc.so.6 server_image/libc.so.6
cp /usr/lib/x86_64-linux-gnu/libsnappy.so.1.1.8 server_image/libsnappy.so.1.1.8
cp /usr/lib/x86_64-linux-gnu/libgflags.so.2.2.2 server_image/libgflags.so.2.2.2
cp /usr/local/lib/libz.so.1.3 server_image/libz.so.1.3
cp /usr/lib/x86_64-linux-gnu/libbz2.so.1.0.4 server_image/libbz2.so.1.0.4
cp /usr/lib/x86_64-linux-gnu/liblz4.so.1.9.3 server_image/liblz4.so.1.9.3
cp /usr/lib/x86_64-linux-gnu/libzstd.so.1.4.8 server_image/libzstd.so.1.4.8
cp /usr/lib/x86_64-linux-gnu/libpthread.so.0 server_image/libpthread.so.0

./scripts/gen_key.sh
mv *.key *.crt server_image/
cd server_image

docker build -t zpods_server:latest .

rm librocksdb.so.8.10.0
rm libstdc++.so.6.0.30
rm libm.so.6
rm libgcc_s.so.1
rm libc.so.6
rm libsnappy.so.1.1.8
rm libgflags.so.2.2.2
rm libz.so.1.3
rm libbz2.so.1.0.4
rm liblz4.so.1.9.3
rm libzstd.so.1.4.8
rm libpthread.so.0

docker tag zpods_server:latest code4love/zpods_server:v1.0
docker push code4love/zpods_server:v1.0

cd ..
