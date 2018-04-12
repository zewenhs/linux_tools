/***************************************************************************************
	gcc -o download_connect download_connect.c -ltelink-usb

	sudo ./download_connect 20000 download.bin // download 测试格式
	sudo ./download_connect // connect 测试格式

	测序开始的两个宏：DOWNLOAD_TEST, CONNECT_TEST 分别对应fw download测试及connect测试
	1. flash download 测试需要将dut_5326_flash_v0002.bin 文件放到运行时的目录中
	2. connect测试时需要修改代码中的mac地址， 其他命令格式请参考本project中附带的文档
 ***************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <telink_usb.h>

#include <unistd.h>
#include <string.h>

#define DOWNLOAD_TEST	0
#define CONNECT_TEST	1

int main(int argc, const char *argv[])
{
#if DOWNLOAD_TEST
	if(argc != 3)
	{
		printf("\nsyntax error!\n./download_connect adr download_fw\n");
		return -1;
	}

	unsigned int addres = strtol(argv[1], NULL, 16);
	const char *file_path = argv[2];
	printf("addres:%x, file_path:%s\n", addres, file_path);

	libusb_device_handle  *dev_handle = telink_usb_open();
	if(dev_handle == NULL)
		printf("---> [FUNC]%s [LINE]:%d dev_handle is NULL\n", __FUNCTION__, __LINE__);

	telink_usb_download(dev_handle, addres, file_path);
	
	telink_usb_close(dev_handle);
#endif


#if CONNECT_TEST
	libusb_device_handle  *dev_handle = telink_usb_open();
	if(dev_handle == NULL)
		printf("---> [FUNC]%s [LINE]:%d dev_handle is NULL\n", __FUNCTION__, __LINE__);

	unsigned char data[6];//MAC
	data[0] = 0xfd;
	data[1] = 0xe1;
	data[2] = 0xe2;
	data[3] = 0xe3;
	data[4] = 0xe4;
	data[5] = 0xc7;
	int ret = telink_usb_action(dev_handle, CONNECT, data);//connect
	if(ret)
		printf("usb transfer error!!!\n");	
	
	unsigned char buf[1024] = {0};
	int size = -1;
	int i;

	ret = telink_usb_get_data(dev_handle, buf, 1024, &size);//get connect result
	if(ret)
		printf("usb transfer error!!!\n");
	for(i = 0; i < size; i++)
	{
		printf("%x:", buf[i]);
	}	
	if(strncmp(data, buf, 6) == 0)
		printf("\nconnect successful!\n");
	else
		printf("\nconnect failed\n");

	printf("now sleep 10s...ret=%d\n", ret);
	sleep(10);
	ret = telink_usb_action(dev_handle, DISCONNECT, data);//disconnect
	if(ret)
		printf("usb transfer error!!!\n");	

	ret = telink_usb_get_data(dev_handle, buf, 1024, &size);//get disconnect result
	if(ret)
		printf("usb transfer error!!!\n");
	if(buf[0] == 0x13)//断开原因码， 0X13为user主动断开， 全部原因码请查看ble_common.h中 ble_sts_t枚举变量
		printf("disconnect successful!\n");
	else
		printf("disconnect failed!\n");

	printf("now sleep 1s...ret=%d\n", ret);
	sleep(1);
	telink_usb_close(dev_handle);//close libusb

#endif
	
	return 0;
}
