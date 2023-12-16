import subprocess
import os
import sys

def generate_commands(binary_path, target_dir):
    try:
        # 获取ldd输出
        ldd_output = subprocess.check_output(["ldd", binary_path]).decode()

        # 解析ldd输出并存储路径
        real_paths = []
        link = {}
        for line in ldd_output.splitlines():
            parts = line.split('=>')
            if len(parts) == 2:
                path = parts[1].split('(')[0].strip()
                if path:
                    real_path = os.path.realpath(path)

                    if os.path.islink(path):
                        link[real_path] = path

                    real_paths.append(real_path)

        # 生成本地拷贝指令和Dockerfile COPY指令
        local_cp_commands = []
        local_rm_commands = []
        docker_cp_commands = []
        for path in real_paths:
            filename = os.path.basename(path)
            target_path = os.path.join(target_dir, filename)
            local_cp_commands.append(f"cp {path} {target_path}")
            local_rm_commands.append(f"rm {target_path}")
            lib_path = os.path.join("/usr/lib", filename)
            docker_cp_commands.append(f"COPY {filename} {lib_path}")
            if path in link:
                link_name = os.path.basename(link[path])
                link_path = os.path.join("/usr/lib", link_name)
                docker_cp_commands.append(f"RUN rm -rf {link_path} && ln -s {lib_path} {link_path}")

        return local_cp_commands, docker_cp_commands, local_rm_commands

    except subprocess.CalledProcessError as e:
        return ["ERROR: ldd command failed"], []

# 接收命令行参数
if len(sys.argv) != 3:
    print("Usage: python gen_copy_cmds.py <binary_path> <target_dir>")
    sys.exit(1)

binary_path = sys.argv[1]
target_dir = sys.argv[2]

# 生成指令
local_cp_cmds, docker_cp_cmds, local_rm_commands = generate_commands(binary_path, target_dir)

# 打印生成的本地拷贝指令
print("Local copy commands:")
for cmd in local_cp_cmds:
    print(cmd)

# 打印生成的Dockerfile COPY指令
print("\nDockerfile COPY commands:")
for cmd in docker_cp_cmds:
    print(cmd)

# 打印生成的本地rm指令
print("\nLocal remove commands:")
for cmd in local_rm_commands:
    print(cmd)

