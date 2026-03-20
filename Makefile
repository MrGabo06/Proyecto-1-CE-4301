start:
	./run.sh

build:
	cd src && ./build.sh

qmu:
	cd src && ./run-qemu.sh

bqmu:
	cd src && ./build.sh
	cd src && ./run-qemu.sh

run:
	docker exec -it rvqemu /bin/bash

dg:
	cd src && gdb-multiarch main.elf -ex "target remote :1234"



