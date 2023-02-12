ssd1306oled-m += module.o i2c.o ssd1306.o chardev.o
obj-m += ssd1306oled.o
 
KDIR = /lib/modules/$(shell uname -r)/build
  
all:
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean

install: uninstall
	insmod ssd1306oled.ko

uninstall:
	-rmmod ssd1306oled