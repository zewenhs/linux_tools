all: cmdfunc.h cmdfunc.c main.c usbprn.h usbprn.c  config.h
	#gcc cmdfunc.c main.c  usbprn.c -o tcbdb  -lusb-1.0
	gcc cmdfunc.c main.c  usbprn.c -o tcbdb -std=gnu99 -lusb-1.0

clean:
	@rm tcbdb 2>/dev/null
