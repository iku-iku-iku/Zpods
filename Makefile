.PHONY: clean test configure build install_cli uninstall hdfs local_deploy format local_deploy light_deploy docker_build AppImage gen_key gen_key_local push_docker docker_run_server docker_run_cli

configure:
	chmod u+x scripts/*.sh
	./scripts/configure.sh
	./scripts/install_rocksdb.sh
	./scripts/install_grpc.sh

gen_key:
	./scripts/gen_key.sh

gen_key_local:
	./scripts/gen_key_local.sh

build:
	./scripts/build.sh

docker_build: build
	./scripts/docker_build_server.sh

push_docker: docker_build
	docker tag zpods_server:latest code4love/zpods_server:v1.0
	docker push code4love/zpods_server:v1.0

docker_run_server: docker_build
	./scripts/run_server.sh

AppImage: build
	./scripts/build_appimage.sh

docker_run_cli: build
	./scripts/docker_build_cli.sh
	docker run --network host -it zpods_cli:latest /bin/bash

local_deploy: build
	bash ./scripts/deploy_local.sh

light_deploy: build
	bash ./scripts/light_deploy.sh

uninstall:
	rm -rf /usr/local/bin/zpods.cli

format:
	@clang-format -i -style=file $(shell find ZpodsLib network \( -name "*.h" -o -name "*.cpp" \) -size -100k)

install_cli: build uninstall AppImage
	@# if there is no /usr/local/bin/zpods.cli, then create a symbol link
	@chmod u+x zpods_cli-x86_64.AppImage
	@[ -f /usr/local/bin/zpods.cli ] || ln -s $(shell pwd)/zpods_cli-x86_64.AppImage /usr/local/bin/zpods.cli
	@echo "installed successfully in /usr/local/bin/zpods.cli!"
	@echo "you can use 'zpods.cli --help' to learn how to use zpods cli"

clean:
	rm -rf build

test: build
	./scripts/test.sh

hdfs:
	stop-dfs.sh
	hdfs namenode -format
	start-dfs.sh
