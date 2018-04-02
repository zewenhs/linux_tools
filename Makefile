CC = gcc
LIB_NAME = telink-usb

all : lib_telink-usb

lib_telink-usb:
	$(CC) -shared -fpic -o lib${LIB_NAME}.so telink_usb.c cmdfunc.c usbprn.c -std=gnu99 -lusb-1.0

install:
	install -m 0755 lib${LIB_NAME}.so /usr/local/lib64/
	install -m 0755 telink_usb.h /usr/local/include/

clean:
	rm -rf *.o
	rm -rf *.so

.PHONY: all clean install
