all: 
	make make_src
	make make_tests

make_tests: make_src
	cp bin/raise tests
	cd tests && make
	
make_src:
	mkdir -p bin
	cd src && make
	
clean:
	cd tests && make clean
