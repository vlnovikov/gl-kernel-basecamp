CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc
MAJOR = 201

obj-m := tick.o interv.o hrtimer.o mytimer.o

all: default clean

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules