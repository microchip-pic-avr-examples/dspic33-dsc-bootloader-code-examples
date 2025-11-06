<picture>
    <source media="(prefers-color-scheme: dark)" srcset="images/microchip_logo_white_red.png">
	<source media="(prefers-color-scheme: light)" srcset="images/microchip_logo_black_red.png">
    <img alt="Microchip Logo." src="images/microchip_logo_black_red.png">
</picture>

# Overview
This repository covers boot loading and firmware update solutions using the Universal Host Bootloading Application (UBHA) protocol/tool. For many dsPIC devices, this is supported through the MPLAB(R) Code Configurator (MCC). MCC can be used to generate code examples for various supported devices and provide customization and configuration options not present in the examples on this site. 

NOTE: For dsPIC33A family boot loading and firmware update solutions, refer to [dspic-bootloader-mdfu repository](https://github.com/microchip-pic-avr-examples/dspic-bootloader-mdfu).

NOTE: For dsPIC Microchip Device Firmware Update (MDFU) based solutions, refer to [dspic-bootloader-mdfu repository](https://github.com/microchip-pic-avr-examples/dspic-bootloader-mdfu).

## <u>dsPIC33 CAN/CAN-FD Bootloader Demo</u>

### Summary
An example bootloader using the 16-bit MCC bootloader and the CAN or CAN-FD peripheral on the dsPIC33 Touch CAN LIN Curiosity Development board demonstrating the following: 
* Immutable secure boot via CodeGuard
* A/B type boot loading with Flash partitioning to support two firmware images and anti-roll back
* Secure boot authentication via ECDSA signature verification
* Secure firmware update via CAN-FD
* Crypto acceleration via TA100
* (Optional) Permanent flash memory locking via ICSP inhibit 

### Related Documentation
* [16bit MCC Bootloader](https://www.microchip.com/16-bit-bootloader)
* [dsPIC33C Touch-CAN-LIN Curiosity Development Board (EV97U97A)](https://www.microchip.com/en-us/development-tool/EV97U97A)

### Setup And Operation
See [secure_boot_and_secure_firmware_upgrade_over_canfd](secure_boot_and_secure_firmware_upgrade_over_canfd/) and the associated [README.md](secure_boot_and_secure_firmware_upgrade_over_canfd/README.md) files for details on the required hardware, software, setup, and how to run the demo. 

# Trademark
MPLAB is a register trademark of Microchip Technology Inc.
