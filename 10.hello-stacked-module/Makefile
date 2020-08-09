# obj-m -> modules to compile (dynamic compilation)
# obj-y -> Static component to compile
# obj-n -> Not to be compiled
obj-m := export.o import.o

all:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
