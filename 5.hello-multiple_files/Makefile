# obj-m -> modules to compile (dynamic compilation)
# obj-y -> Static component to compile
# obj-n -> Not to be compiled

hello-objs :=file1.o file2.o
obj-m := hello.o

hello.ko:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
