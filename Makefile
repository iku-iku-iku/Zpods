.PHONY: clean test configure

configure:
	sudo apt install ceph librados2 librados-dev -y
	sudo apt install libssl3 libssl-dev -y
	git submodule update --init --recursive
	chmod a+x ./test.sh

clean:
	rm -rf build

test: 
	./test.sh

hdfs:
	stop-dfs.sh
	hdfs namenode -format
	start-dfs.sh
