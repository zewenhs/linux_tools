1. Install the dependences libudev and libusb1
    For Debain/Ubuntu serials/distributions:
        sudo apt-get install libudev-dev libusb-1.0-0-dev libusb-dev

    For Redhat/Fedora serials/distributions:
        sudo yum install libusb-devel.i686 libusb-devel.x86_64

2. copy the binary file into system path
    sudo cp tcbdb /usr/bin/
    sudo chmod a+x /usr/bin/tcbdb

3. Run without root permission handle
    If you want to use the binary tools without the root permission, do following commands
        sudo cp 60-tcbdb-usb.rules /etc/udev/rules.d/60-tcbdb-usb.rules
        sudo udevadm control --reload-rules

    remove the USB debug device and re-insert it

