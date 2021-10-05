CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

obj-m := messanger.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
test:
	dmesg -C
	insmod messanger.ko
	echo 500 4000 "*" > /sys/class/messanger/messanger
	echo 250 2000 "**" > /sys/class/messanger/messanger
	echo 125 1000 "***" > /sys/class/messanger/messanger
	sleep 5
	rmmod messanger
	dmesg | grep -v audit