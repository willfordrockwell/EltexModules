obj-m += my_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

check:
	/lib/modules/$(shell uname -r)/build/scripts/checkpatch.pl -f my_module.c --no-tree
