<picture>
    <source media="(prefers-color-scheme: dark)" srcset="images/microchip_logo_white_red.png">
	<source media="(prefers-color-scheme: light)" srcset="images/microchip_logo_black_red.png">
    <img alt="Microchip Logo." src="images/microchip_logo_black_red.png">
</picture>

## dspic33 dsc bootloader code examples

![Board picture](./images/board.jpg)

## Summary
Demonstrates the following: 
* Immutable secure boot via CodeGuard
* A/B type bootloading with Flash partitioning to support two firmware images and anti-roll back
* Secure boot authentication via ECDSA signature verification 
* Secure firmware update via CAN-FD
* Crypto acceleration via TA100

## Related Documentation
* [dsPIC33C Touch-CAN-LIN Curiosity Development Board User's Guide](https://www.microchip.com/en-us/development-tool/EV97U97A)
* [dsPIC33CK1024MP710 Datasheet](https://www.microchip.com/en-us/product/dsPIC33CK1024MP710) 

## Software Used 
* [MPLAB® X IDE v6.15](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide) or later
* [MPLAB® XC16 v2.10](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers) or later
* Device Family Pack: TBD
* [MPLAB® Code Configurator (MCC) v5.6.1](https://www.microchip.com/en-us/tools-resources/configure/mplab-code-configurator) or later
* 16-Bit Bootloader MCC module v1.25.0 or later
* CryptoAuthentication Library MCC module v5.7.0 or later
* Trust Anchor Library MCC module v1.2.0 or later
* [Universal Bootloader Host Application](www.microchip.com/16-bit-bootloader)

## Hardware Used
* [dsPIC33C Touch-CAN-LIN Curiosity Development Board (EV97U97A)](https://www.microchip.com/en-us/development-tool/EV97U97A)

## Setup
##### Hardware Setup
1. Connect the micro-USB cable to port J1 of the dsPIC33 Touch-CAN-LIN Curiosity Development Board to the host computer.

##### MPLAB® X IDE Setup
1. Open the boot.X project in MPLAB® X IDE
2. Build and program the device
3. Open the app.X project in MPLAB® X IDE
4. Build (do not program) the device

##### Universal Bootloader Host Application Setup
1. Under Device Architecture, select PIC24 MCUs\dsPIC33 DSCs
2.  Under Protocol, select CAN 
3.  Go to Settings and select CAN from the dropdown to open the CAN Settings window 
4.  Verify the CAN Module is present under the CAN Module dropdown 
5. Check the CAN-FD checkbox
6. Check the Data Bit Rate Enable box
7. Set the data bitrate to 2.00 Mbits/s if it is not set to it by default
8. Click Apply
9. Go to File and select Open/Load File (*.hex)
10. Navigate to the app.X project and select the .hex file under dist --> default --> production
11. Click "Read Device Settings"
12. Click "Program Device"

## Operation



