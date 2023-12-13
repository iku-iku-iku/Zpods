#!/bin/bash
# 生成服务端密钥和证书
openssl genrsa -out server.key 2048
openssl req -new -x509 -sha256 -key server.key -out server.crt -days 365 -subj "/C=CN/ST=四川省/L=成都市/O=电子科技大学/OU=zpods/CN=localhost"

# 生成客户端密钥和证书
openssl genrsa -out client.key 2048
openssl req -new -x509 -sha256 -key client.key -out client.crt -days 365 -subj "/C=CN/ST=四川省/L=成都市/O=电子科技大学/OU=zpods/CN=localhost"
