/*************************************************************************
	> File Name: telink_usb.h
	> Author: zewen
	> Mail: zewen.zhu@telink.com
	> Created Time: Mon 02 Apr 2018 03:36:37 PM CST
 ************************************************************************/

#pragma once

#include <libusb-1.0/libusb.h>

libusb_device_handle  *telink_usb_open();

void telink_usb_close(libusb_device_handle *hDev);

int telink_usb_download(libusb_device_handle *hDev, unsigned int adr, const char *file_path);
