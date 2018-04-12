# dongle_linux
dongle linux   

```
make 
sudo make install
export LD_LIBRARY_PATH="/usr/local/lib64"
sudo ldconfig
```
# 说明
1. 如果是download fw到flash的话， 应用程序需要传入2个参数， 第一个参数是需要下载到flash 的地址， 第二个参数是需要下载fw的路径， 并且需要将 dut_5326_flash_v0002.bin 放到运行程序的当前路径下. 这个fw可以响应PC发过来的某些指令， 这些指令是通过libusb_control_transfer 传送的。而且这个fw是被写到dongle的SRAM中运行，重启后不再存在, 此种情况与dongle中原来下载的fw无关
2. flash download 功能与芯片型号有关，目前的程序在8266/8267上验证OK 
3. 如果是scan/connect/ota等功能需要dongle下载本project中 doc/8266_master_kma_dongle.bin， 或者后续SDK 有更新后提供新的正式的bin，注意新的bin返回信息的格式是否有变化。 这些命令是通过libusb_bulk_transfer传送的

**Install the dependences libudev and libusb1**  
For Debain/Ubuntu serials/distributions:  
```
sudo apt-get install libudev-dev libusb-1.0-0-dev libusb-dev  
```
**Run without root permission handle**  
If you want to use the binary tools without the root permission, do following commands
```
sudo cp 60-tcbdb-usb.rules /etc/udev/rules.d/60-tcbdb-usb.rules
sudo udevadm control --reload-rules
```
remove the USB debug device and re-insert it

# libusb errorno
```
enum libusb_error {
	/** Success (no error) */
	LIBUSB_SUCCESS = 0,

	/** Input/output error */
	LIBUSB_ERROR_IO = -1,

	/** Invalid parameter */
	LIBUSB_ERROR_INVALID_PARAM = -2,

	/** Access denied (insufficient permissions) */
	LIBUSB_ERROR_ACCESS = -3,

	/** No such device (it may have been disconnected) */
	LIBUSB_ERROR_NO_DEVICE = -4,

	/** Entity not found */
	LIBUSB_ERROR_NOT_FOUND = -5,

	/** Resource busy */
	LIBUSB_ERROR_BUSY = -6,

	/** Operation timed out */
	LIBUSB_ERROR_TIMEOUT = -7,

	/** Overflow */
	LIBUSB_ERROR_OVERFLOW = -8,

	/** Pipe error */
	LIBUSB_ERROR_PIPE = -9,

	/** System call interrupted (perhaps due to signal) */
	LIBUSB_ERROR_INTERRUPTED = -10,

	/** Insufficient memory */
	LIBUSB_ERROR_NO_MEM = -11,

	/** Operation not supported or unimplemented on this platform */
	LIBUSB_ERROR_NOT_SUPPORTED = -12,

	/* NB: Remember to update LIBUSB_ERROR_COUNT below as well as the
	   message strings in strerror.c when adding new error codes here. */

	/** Other error */
	LIBUSB_ERROR_OTHER = -99,
};
```
