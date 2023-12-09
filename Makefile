.PHONY: clean test configure build install_cli uninstall hdfs

configure:
	sudo apt install ceph librados2 librados-dev -y
	sudo apt install libssl3 libssl-dev -y
	git submodule update --init --recursive
	chmod a+x ./scripts/test.sh
	chmod a+x ./scripts/build.sh

build:
	./scripts/build.sh
	cp build/network/client/cli/src/zpods_cli .
	cp build/network/server/src/zpods_server .
uninstall:
	rm -rf /usr/local/bin/zpods.cli

install_cli: build uninstall
	@# if there is no /usr/local/bin/zpods.cli, then create a symbol link
	@[ -f /usr/local/bin/zpods.cli ] || ln -s $(shell pwd)/build/client/cli/src/zpods_cli /usr/local/bin/zpods.cli
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
