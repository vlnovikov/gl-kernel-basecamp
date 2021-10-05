#
# sysfs examples
#

BUILD_KERNEL = /lib/modules/$(shell uname -r)/build

ifneq ($(KERNELRELEASE),)

obj-m += exchange.o
# obj-m += rw.o

else

KERNELDIR := $(BUILD_KERNEL)

.PHONY: all clean
all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

endif
