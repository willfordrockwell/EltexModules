obj-m += my_module.o

all: check build load test unload

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

check:
	/lib/modules/$(shell uname -r)/build/scripts/checkpatch.pl -f my_module.c --no-tree

load:
	sudo insmod my_module.ko

unload:
	sudo rmmod my_module

test:
	sudo head -c20 /dev/test_module0
