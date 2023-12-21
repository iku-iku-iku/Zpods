#!/bin/bash

echo "***building client***"

cp client.key *.crt AppDir/

cp build/network/client/cli/src/zpods_cli AppDir/usr/bin/

# you need run `python3 scripts/gen_cmd_for_dep.py deploy/zpods_cli AppDir/usr/lib` to get following commands
cp ./build/ZpodsLib/src/libzpods_lib.so AppDir/usr/lib/libzpods_lib.so
cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30 AppDir/usr/lib/libstdc++.so.6.0.30
cp /usr/lib/x86_64-linux-gnu/libm.so.6 AppDir/usr/lib/libm.so.6
cp /usr/lib/x86_64-linux-gnu/libgcc_s.so.1 AppDir/usr/lib/libgcc_s.so.1
cp /usr/lib/x86_64-linux-gnu/libc.so.6 AppDir/usr/lib/libc.so.6
cp /usr/lib/x86_64-linux-gnu/libcrypto.so.3 AppDir/usr/lib/libcrypto.so.3

appimagetool AppDir

# clean
rm AppDir/usr/bin/zpods_cli

# you need run `python3 scripts/gen_cmd_for_dep.py deploy/zpods_cli AppDir/usr/lib` to get following commands
rm AppDir/usr/lib/libzpods_lib.so
rm AppDir/usr/lib/libstdc++.so.6.0.30
rm AppDir/usr/lib/libm.so.6
rm AppDir/usr/lib/libgcc_s.so.1
rm AppDir/usr/lib/libc.so.6
rm AppDir/usr/lib/libcrypto.so.3
