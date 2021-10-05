CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

obj-m := reminder.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
test:
	dmesg -C
	insmod reminder.ko
	echo user1 2 hello > /sys/class/records_writer/recorder
	echo user2 5 my > /sys/class/records_writer/recorder
	echo user3 3 friend > /sys/class/records_writer/recorder
	echo user4 4 little > /sys/class/records_writer/recorder
	sleep 7
	rmmod reminder
	dmesg | grep -v audit
