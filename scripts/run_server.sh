#!/bin/bash

PORT=50051

rm server_log

# if port $PORT is in use, then kill the process that uses it
sudo pkill docker
mkdir -p /tmp/server_storage

docker run --rm --name zpods_server \
	-p $PORT:$PORT \
	-v /tmp/server_storage:/app/server_storage \
	zpods_server >server_log 2>&1 &

tail -f server_log
