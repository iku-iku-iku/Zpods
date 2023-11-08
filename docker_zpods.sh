#!/bin/bash
# run_zpods.sh

# 使用"$@"传递所有参数
docker run --rm -v "$(pwd):/workspace" -w /workspace zpods "$@"

