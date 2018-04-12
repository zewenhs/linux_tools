/**************************************************************************************************************************
	gcc -o scan scan.c -ltelink-usb -lpthread
	./scan
	本示例通过向dongle发送命令实现扫描周围所有的蓝牙设备并在PC上显示出扫描到的蓝牙设备的MAC 及广播报信息，
	另外本示例中也展示如何通过名字来确定是否有扫到telink的蓝牙设备（这个与广播包的格式有关， 本例中只限telink的蓝牙设备）
	可以根据需要更改代码中的"thid"字符串来做尝试
 **************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <telink_usb.h>

#include <unistd.h>
#include <pthread.h>
#include <string.h>

int stop_thd = 0;
int device_num = 0;
libusb_device_handle *dev_handle;
unsigned char buf[1024] = {0};
int size = -1;
unsigned char *pstr[100];

/*
 * 保存scan到的data， 相同的数据只会保存一次
 * */
int store_scan_data(unsigned char *pstr[], unsigned char *buf, int num, int size)
{
	int i = 0, j = 0;

	if(num > 100)
	{
		printf("fatal error! max support is 100 scan device\n");
		return -1;
	}
	if(num == 0)
	{
		pstr[0] = (unsigned char *) malloc(size);
		if(pstr[0] == NULL)
		{
			printf("malloc is null\n");
			return -1;
		}
		memcpy(pstr[0], buf, size);
		return 0;
	}
	
	for(i = 0; i < num; i++)
	{
		for(j = 0; j < 6; j++)
		{
			if(*(pstr[i] + j) != buf[j])
				break;
		}
		if(j == 6)
			return 1; //have same string
	}
	//not have same string 
	pstr[num] = (unsigned char *) malloc(size);
	if(pstr[num] == NULL)
		printf("malloc is NULL 1111\n");
	memcpy(pstr[num], buf, size);
	return 0;

}

void *get_scan_data(void *arg)
{
	printf("this is a new thread!\n");
	int i = 0;
	int ii = 0;
	int t = 0;
	int ret = -1;
	int num = 0;

	while(1)
	{
		t = 0;
		if(stop_thd)
		{
			break;
		}
		ret = telink_usb_get_data(dev_handle, buf, 1024, &size);
		if(ret != 0)
			printf("bulk transfer error!\n");

		ret = store_scan_data(pstr, &buf[1], num, size - 1);
		if(ret == 0)
			num++;
		
		if(ret == 0)
		{
			printf("\n------------------------------------------------ size:%d\n", size);
			for( i = 1; i < size; i++ )
				printf("%x ", buf[i]);
		}
	}
	device_num = num;
	printf("\nleave new thread!\n");
}

int main(int argc, const char *argv[])
{
	dev_handle = telink_usb_open();
	if(dev_handle == NULL)
		printf("dev_handle is NULL\n");

	int ret = telink_usb_action(dev_handle, SCAN_ON, NULL);
	if(ret)
		printf("usb transfer error!!!\n");	


	pthread_t thread;
	ret = pthread_create(&thread, NULL, get_scan_data, NULL);
	if(ret != 0)
		printf("pthread create failed!!!\n");
			
	printf("now sleep 20s...ret=%d\n", ret);
	sleep(20);
	stop_thd = 1;
	printf("now will sleep for scan data\n");
	sleep(1);
	printf("now will scan off\n");
	
	ret = telink_usb_action(dev_handle, SCAN_OFF, NULL);
	if(ret)
		printf("usb transfer error!!!\n");	
	
	printf("waiting for sub thread exit stop_thd:%d\n", stop_thd);
	ret = pthread_join(thread, NULL);

	/* 如果是telink的产品scan 返回的数据如下面所示（仅展示前面的部分数据）：前面6的字节是MAC 地址，仅接着的一个字节是广播包的总长度，
	 * 再往后一个字节是设备名字字符串长度+1（假设为n）,第九个字节是固定值9, 后面接着的n-1个字节就是设备的名字 
	 *
	 * fd e1 e2 e3 e4 c7 13 5 9 74 68 69 64 ... //此设备的名字为 "thid", MAC:C7:E4:E3:E2:E1:FD
	 *
	 * */
	unsigned char name_buf[20] = {0};
	int i = 0, j = 0;
	printf("scan device num:%d\n", device_num);
	for(i = 0; i < device_num; i++)
	{
		if(*(pstr[i] + 8) != 9)
			continue;
		int char_num = *(pstr[i] + 7) - 1;
		memcpy(name_buf, (pstr[i] + 9), char_num);
		name_buf[char_num] ='\0';
		
		if(strcmp(name_buf, "thid") == 0)
		{
			printf("thid found: ");
			for(j = 5; j > 0; j--)
				printf("%x:", *(pstr[i] + j));
			printf("%x\n", *(pstr[i] + j));

		}
	}

	for(i= 0; i < device_num; i++)
		free(pstr[i]);

	printf("now sleep 1s...ret=%d\n", ret);
	sleep(1);
	telink_usb_close(dev_handle);

	return 0;
}
