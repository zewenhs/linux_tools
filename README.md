# dongle_linux
dongle linux   

```
make 
sudo make install
export LD_LIBRARY_PATH="/usr/local/lib64"
sudo ldconfig
```

如果是download fw到flash的话， 应用程序需要传入2个参数， 第一个参数是需要下载到flash 的地址， 第二个参数是需要下载fw的路径， 并且需要将 dut_5326_flash_v0002.bin 放到当前路径下.  

Install the dependences libudev and libusb1
    For Debain/Ubuntu serials/distributions:
        sudo apt-get install libudev-dev libusb-1.0-0-dev libusb-dev  

Run without root permission handle
    If you want to use the binary tools without the root permission, do following commands
        sudo cp 60-tcbdb-usb.rules /etc/udev/rules.d/60-tcbdb-usb.rules
        sudo udevadm control --reload-rules

    remove the USB debug device and re-insert it

