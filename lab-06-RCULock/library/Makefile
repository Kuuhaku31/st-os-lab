#ARCH=x86
#ARCH=arm64

PWD=$(shell pwd)
 
KERNEL_DIR = /lib/modules/$(shell uname -r)/build

obj-m += kylin_rcu.o
 
all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	rm -rf *.o *.cmd *.mod.c *.mod
	rm -rf  *~ core .depend  .tmp_versions Module.symvers modules.order -f 
	rm -f .*ko.*  *ko.*  .*.cmd .*.o.cmd *.ko 

#make -C /path/to/kernel/tree M=$PWD modules
