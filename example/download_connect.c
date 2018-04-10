/*************************************************************************
	gcc -o test test.c -ltelink-usb
	sudo ./test 20000 download.bin
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <telink_usb.h>

#include <unistd.h>

#define DOWNLOAD_TEST	1
#define CMD_TEST	0

int main(int argc, const char *argv[])
{
#if DOWNLOAD_TEST
	if(argc != 3)
	{
		printf("\nsyntax error!\n./tcbdb adr download_fw\n");
		return -1;
	}

	unsigned int addres = strtol(argv[1], NULL, 16);
	const char *file_path = argv[2];
	printf("addres:%x, file_path:%s\n", addres, file_path);

	libusb_device_handle  *dev_handle = telink_usb_open();
	if(dev_handle == NULL)
		printf("zewen---> [FUNC]%s [LINE]:%d dev_handle is NULL\n", __FUNCTION__, __LINE__);

	telink_usb_download(dev_handle, addres, file_path);
	
	telink_usb_close(dev_handle);
#endif


#if CMD_TEST
	libusb_device_handle  *dev_handle = telink_usb_open();
	if(dev_handle == NULL)
		printf("zewen---> [FUNC]%s [LINE]:%d dev_handle is NULL\n", __FUNCTION__, __LINE__);

	unsigned char data[6];
	data[0] = 0xb1;
	data[1] = 0xe1;
	data[2] = 0xe2;
	data[3] = 0xe3;
	data[4] = 0xe4;
	data[5] = 0xc7;
	int ret = telink_usb_action(dev_handle, CONNECT, data);
	if(ret)
		printf("usb transfer error!!!\n");	
	printf("now sleep 30s...ret=%d\n", ret);
	sleep(30);
	ret = telink_usb_action(dev_handle, DISCONNECT, data);
	if(ret)
		printf("usb transfer error!!!\n");	
	printf("now sleep 1s...ret=%d\n", ret);
	sleep(1);
	telink_usb_close(dev_handle);

#endif
	
	return 0;
}
