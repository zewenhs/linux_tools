/*************************************************************************
	> File Name: telink_usb.h
	> Author: Telink
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

/**
 *@RETURN: a device handle for the first found telink device, or NULL on error or if the device could not be found.
 *eg: if your pc have a evk and 8266 dongle inserted at the same time, the evk may be opened, because this function 
 *		only distinguish the vendor id, not the the product id.
 * **/
libusb_device_handle  *telink_usb_open();

/**
 *@hDev: usb device handle return by telink_usb_open()
 * **/
void telink_usb_close(libusb_device_handle *hDev);

/**
 *@hDev: usb device handle return by telink_usb_open()
 *@adr: flash address to be written
 *@file_path: download fw path
 *
 *@RETURN: 0 on success; other failed
 * **/
int telink_usb_download(libusb_device_handle *hDev, unsigned int adr, const char *file_path);

/**
 *@hDev: usb device_handle return by telink_usb_open()
 *@cmd: command defined by TL_CMDType
 *@data: some parameter if needed(eg, MAC address when use CONNECT cmd)
 *
 * @RETURN: 0 on success; other see libusb_error
 * **/
int telink_usb_action(libusb_device_handle *hDev, TL_CMDType cmd, unsigned char *data);

/**
 *@hDev: usb device_handle return by telink_usb_open()
 *@buf: data buffer for output
 *@len: the maximum number of bytes to receive into the buf
 *@size: the number of bytes actually transferred
 *
 * @RETURN: 0 on success; other see libusb_error
 * **/
int telink_usb_get_data(libusb_device_handle *hDev, unsigned char *buf, int len, int *size);
