/*************************************************************************
	> File Name: telink_usb.h
	> Author: zewen
	> Mail: zewen.zhu@telink.com
	> Created Time: Mon 02 Apr 2018 03:36:37 PM CST
 ************************************************************************/

#pragma once

#include <libusb-1.0/libusb.h>

typedef enum {
	SCAN_ON,
	SCAN_OFF,
	CONNECT,
	DISCONNECT,
	OTA,
	BAT_STATUS
}TL_CMDType;

libusb_device_handle  *telink_usb_open();

void telink_usb_close(libusb_device_handle *hDev);
/**
 *@hDev: usb device handle return by telink_usb_open()
 *@adr: flash address to be written
 *@file_path: download fw path
 * **/
int telink_usb_download(libusb_device_handle *hDev, unsigned int adr, const char *file_path);

/**
 *@hDev: usb device_handle return by telink_usb_open()
 *@cmd: command defined by TL_CMDType
 *@data: some parameter if needed(eg, SCAN enable or disable; MAC address when use CONNECT cmd)
 * **/
int telink_usb_action(libusb_device_handle *hDev, TL_CMDType cmd, unsigned char *data);

int telink_usb_get_data(libusb_device_handle *hDev, unsigned char *buf, int len, int *size);
