.PHONY: clean test configure

configure:
	git submodule update --init --recursive
	chmod a+x ./test.sh

clean:
	rm -rf build

test: 
	./test.sh

