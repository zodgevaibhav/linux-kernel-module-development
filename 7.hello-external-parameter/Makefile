# obj-m -> modules to compile (dynamic compilation)
# obj-y -> Static component to compile
# obj-n -> Not to be compiled
obj-m := hello.o

hello.ko: hello.c
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
