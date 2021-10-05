obj-m += ex02.o test_ex02.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	dmesg -C
	insmod ex02.ko
	-insmod test_ex02.ko
	rmmod ex02.ko
	dmesg
