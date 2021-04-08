# BareMetal-BBB
Bare Metal Application on the BeagleBone<br>
We are using BOOTP (Bootstrap Protocol) and TFTP to transfer a Boot image (binary) and run it.
*The boot image is downloaded directly into internal RAM at the location 0x402F0400 on GP devices. The maximum size of downloaded image is 109 KB.*

## Setup
### StarterWare
- Download Linaro GCC Compiler: https://launchpad.net/gcc-arm-embedded/4.7/4.7-2012-q4-major <br>
  (Version 9 works too: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- Download StarterWare installer for AM335x and BeagleBone Black patch from http://www.ti.com/tool/starterware-sitara <br>
- Build the libraries. The easiest way is to build all examples:<br>

```makefile
# path/to/AM335X_StarterWare/build/armv7a/gcc/makedefs:
LIB_PATH=${HOME}/src/gcc-arm-none-eabi-9-2020-q2-update/
LIB_GCC=${LIB_PATH}/lib/gcc/arm-none-eabi/9.3.1/
LIB_C=${LIB_PATH}/arm-none-eabi/lib/
GCCR=${HOME}/src/gcc-arm-none-eabi-9-2020-q2-update/bin
PREFIX=arm-none-eabi-
```
`/path/to/AM335X_StarterWare/build/armv7a/gcc/am335x/beaglebone$ make all`<br>

### Network Manager
Assign the static IP `192.168.8.1/24` to the `enp0s4f0u3` interface (BBB - RNDIS Ethernet over USB) <br>
c.f. [01-bbb.rules](01-bbb.rules)

### firewall
```
# In case you are using iptables:
# iptables.conf:
-A INPUT -p udp --dport 67:68 --sport 67:68 -j ACCEPT
-A INPUT -p udp -s 192.168.8.2 -d 192.168.8.1 -j ACCEPT
-A OUTPUT -p udp -s 192.168.8.1 -d 192.168.8.2 -j ACCEPT
-A OUTPUT -p udp --sport 67:68 --dport 67:68 -j ACCEPT
```

### Hardware
Connect a 10K pulldown resistor between `P8_43` and GND; this will make the boot order: `SPI0, MMC0 (SD card), USB0, UART0`.
Without an SD card (and no SPI device present), USB0,  will be the boot device chosen. The ROM code implements the RNDIS class driver, and from a user's perspective, USB boot is indistinguishable from Ethernet boot. (Note: for EMAC boot, pullup `P8_45`)
#### UART Console:
```
BBB (J1)       FTDI
───────┐      ┌─────
DGND  1├──────┤ GND
NC    2│      │ CTS
NC    3│      │ 5V
RX    4├──────┤ TX
TX    5├──────┤ RX
NC    6│      │ DTR
```
Connect USB0 (P4) to a host computer.
### dnsmasq.conf
`dhcp-vendorclass=set:bbrom,AM335x ROM` BOOTP Identifier string, from EMAC Boot Procedure, AM335x TRM <br>
`dhcp-range=192.168.8.0,static` must match the subnet assigned to the device <br>
`dhcp-host=a0:f6:fd:8b:6a:c9,192.168.8.2,barebone` this MAC can show up on LOGs from iptables. Different than the one from ifconfig <br>

## Compilation & Run
c.f. [Makefile](Makefile) [test.c](test.c) <br>
```
make all #make test.c
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb  #-parenb means no parity bit
minicom -D /dev/ttyUSB0 #or screen /dev/ttyUSB0, in one terminal
                        # CCCC... on /dev/ttyUSB0 means boot sequence cycling?
                        #                                xmodem does that too
sudo dnsmasq -d -C dnsmasq.conf #on another terminal
# debug: tshark -i <usb-rndis>
# "Hello World" should appear on /dev/ttyUSB0
```














## References
https://github.com/beagleboard/beaglebone-black/wiki/System-Reference-Manual <br>
https://octavosystems.com/app_notes/bare-metal-on-osd335x-using-u-boot/ <br>
https://www.ti.com/lit/ug/spruh73q/spruh73q.pdf <br>
