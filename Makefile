obj-m += epidriver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	@-sudo rmmod epidriver
	@sudo dmesg -C
	@sudo insmod epidriver.ko
	@sudo dmesg

test_level1:
	@-sudo rmmod epidriver
	@sudo dmesg -C
	@sudo insmod epidriver.ko level=1
	@sudo dmesg

test_level2:
	@-sudo rmmod epidriver
	@sudo dmesg -C
	@sudo insmod epidriver.ko level=2
	@sudo dmesg

test_level3:
	@-sudo rmmod epidriver
	@sudo dmesg -C
	@sudo insmod epidriver.ko
	@sudo dmesg

test_level4:
	@-sudo rmmod epidriver
	@sudo dmesg -C
	@sudo insmod epidriver.ko alphabet='abcXYZ0123456789,?\'
	@sudo dmesg
