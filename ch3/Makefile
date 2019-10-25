obj-m += pid.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o simple-shell simple-shell.c
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm simple-shell
