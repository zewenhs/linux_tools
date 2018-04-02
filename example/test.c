/*************************************************************************
	gcc -o test test.c -ltelink-usb
	sudo ./test 20000 download.bin
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <telink_usb.h>

int main(int argc, const char *argv[])
{
	if(argc != 3)
	{
		printf("\nsyntax error!\n./tcbdb adr download_fw\n");
		return -1;
	}

	unsigned int addres = strtol(argv[1], NULL, 16);
	const char *file_path = argv[2];
	printf("addres:%x, file_path:%s\n", addres, file_path);

	libusb_device_handle  *dev_handle = telink_usb_open();
	
	telink_usb_download(dev_handle, addres, file_path);
	
	telink_usb_close(dev_handle);
	
	return 0;
}
