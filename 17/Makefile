########Makefile for Debian 4.0rc3

test = hello_world
obj-m := $(test).o
KERNELDIR = /lib/modules/`uname -r`/build
PWD = `pwd`
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
install:
	insmod $(test).ko
uninstall:
	rmmod $(test).ko
clean:
	rm -rf *.o *.mod.c *.ko
	rm -rf Module.symvers .*cmd .tmp_versions
