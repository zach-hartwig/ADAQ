#=============================================================================
#
#             --- CAEN SpA - Computing Systems Division ---
#
#  Written by  CAEN software division (support.computing@caen.it)
#
#  Makefile
#
#
#=============================================================================


# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS) -I$(LDDINCDIR)

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= CAENUSBdrvB.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINCDIR=$(PWD)/../include modules

endif

install:
	@echo "KERNEL==\"v1718_[0-9]\", SYMLINK+=\"usb/%k\", MODE=\"0666\"" > /etc/udev/rules.d/10-CAEN-USB.rules
	@udevadm control --reload-rules
	@cp CAENUSBdrvB.ko /lib/modules/$(shell uname -r)/kernel/drivers/misc/
	@echo CAENUSBdrvB >> /etc/modules
	@echo "installing CAENUSBdrvB driver.. please wait"
	@depmod -a
	@modprobe CAENUSBdrvB
	@echo "installation done"

uninstall:
	@modprobe -r CAENUSBdrvB
	@rm /etc/udev/rules.d/10-CAEN-USB.rules
	@udevadm control --reload-rules
	@rm /lib/modules/$(shell uname -r)/kernel/drivers/misc/CAENUSBdrvB.ko
	@sed -i '/CAENUSBdrvB/d' /etc/modules
	@depmod -a
	@echo "driver uninstalled"

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
