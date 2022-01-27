# EZ-USB (CY7C68013A EZ-USB FX2LP) based decoder software for Canon HJ14ex4.3B IRSE

## udev (autoloading firmware for EZ-USB)

**/etc/udev/rules.d/90-fx.rules**:
```
SUBSYSTEM=="usb", ACTION=="add", ATTR{idVendor}=="04b4", ATTR{idProduct}=="8613", SYMLINK+="fx2lp-devkit", RUN+="/usr/sbin/fxload -D $tempnode -t fx2lp -I /usr/share/sigrok-firmware/fx2lafw-sigrok-fx2-8ch.fw.hex"
```
make sure **fxload** and **sigrok** packages installed

## diagnostic

after connecting this board to usb port you should notice log messages:
```
[700467.647356] usb 1-1.2: new high-speed USB device number 53 using dwc_otg
[700467.747908] usb 1-1.2: New USB device found, idVendor=04b4, idProduct=8613, bcdDevice=a0.01
[700467.747927] usb 1-1.2: New USB device strings: Mfr=0, Product=0, SerialNumber=0
[700467.748835] usbtest 1-1.2:1.0: FX2 device
[700467.748853] usbtest 1-1.2:1.0: high-speed {control bulk-in bulk-out} tests (+alt)
[700467.845413] usb 1-1.2: USB disconnect, device number 53
[700469.351363] usb 1-1.2: new high-speed USB device number 54 using dwc_otg
[700469.452666] usb 1-1.2: New USB device found, idVendor=1d50, idProduct=608c, bcdDevice= 0.01
[700469.452690] usb 1-1.2: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[700469.452705] usb 1-1.2: Product: fx2lafw
[700469.452720] usb 1-1.2: Manufacturer: sigrok
[700469.452735] usb 1-1.2: SerialNumber: sigrok FX2 8ch
```
and **lsusb** will show:
```
Bus 001 Device 054: ID 1d50:608c OpenMoko, Inc. Fx2lafw
```

## operating
binary start:
```
./ez_usb_rot_dec --freed-target=10.1.5.65:50050 --freed-div=150 --freed-id=8
```
where:
* *--freed-target=10.1.5.65:50050* is a target host:port where FreeD packet will be sent
* *--freed-id=8*  is a camera id in FreeD protocol
* *--freed-div=150* defines a rate for sending UDP packets to FreeD received, this value is a divider of USB transfers counter
* *--log-file-name=/tmp/log* where log will be save
* *--demonize* start in daemon mode

after startup in non-daemon mode it outputs logs and statistic:
```
  [2022-01-27 10:06:27.543] ez_usb_rot_dec-v1.0-86-gd7c67b8, Copyright by Maksym Veremeyenko (c) 2022
  [2022-01-27 10:06:27.544] ez_usb_reader_proc: Searching for 1D50:608C
  [2022-01-27 10:06:27.544] freed_sender_proc: Entering....
! [2022-01-27 10:06:27.548] freed_sender_proc: new target [10.1.5.65:50050]
! [2022-01-27 10:06:27.568] ez_usb_reader_proc: endpoint=130 (0x82)
! [2022-01-27 10:06:27.569] ez_usb_reader_proc: fw_version Major=01, Minor=04, revision 01
! [2022-01-27 10:06:27.586] command_start_acquisition: GPIF delay = 11, clocksource = 48MHz.
  [2022-01-27 10:06:27.586] counter_proc: Entering...
|          0 |          0 |          0| [       27078656  ]
```






