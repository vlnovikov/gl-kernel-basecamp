#
# procfs example
#

TARGET = procfs_rw
BUILD_KERNEL = /lib/modules/$(shell uname -r)/build

ifneq ($(KERNELRELEASE),)
	obj-m := $(TARGET).o
	$(TARGET)-objs := rw.o
else
	KERNELDIR := $(BUILD_KERNEL)

.PHONY: all clean
all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean
test:
	insmod procfs_rw.ko
	echo "hello to my little friend" > /proc/exercise10/buffer
	cat /proc/exercise10/buffer
	rmmod procfs_rw.ko
endif

