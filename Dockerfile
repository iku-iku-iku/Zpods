# 使用官方的 Ubuntu 基础镜像
FROM ubuntu:22.04

# 为了避免在安装过程中出现任何交互式提示
ENV DEBIAN_FRONTEND=noninteractive

# 设置工作目录
WORKDIR /app

# 将编译好的可执行文件复制到容器中
COPY ./cmake-build-debug/client/cli/src/zpods_cli /app/zpods_cli
COPY ./libs/libcrypto.so.3 /usr/lib
COPY ./cmake-build-debug/ZpodsLib/src/libzpods_lib.so /usr/lib

# 给执行文件添加运行权限
RUN chmod +x /app/zpods_cli

# 设置环境变量以便可以从任何路径运行您的CLI
ENV PATH="/app:${PATH}"

# 设置容器启动时默认执行的命令
ENTRYPOINT ["zpods_cli"]
