<picture>
    <source media="(prefers-color-scheme: dark)" srcset="images/microchip_logo_white_red.png">
	<source media="(prefers-color-scheme: light)" srcset="images/microchip_logo_black_red.png">
    <img alt="Microchip Logo." src="images/microchip_logo_black_red.png">
</picture>

## <u>dsPIC33 CAN/CAN-FD Bootloader Demo</u>

### Summary
An example bootloader using the 16-bit MCC bootloader and the CAN or CAN-FD peripheral on the dsPIC33 Touch CAN LIN Curiosity Development board demonstrating the following: 
* Immutable secure boot via CodeGuard
* A/B type bootloading with Flash partitioning to support two firmware images and anti-roll back
* Secure boot authentication via ECDSA signature verification
* Secure firmware update via CAN-FD
* Crypto acceleration via TA100
* (Optional) Permanent flash memory locking via ICSP inhibit 

### Related Documentation
* [16bit MCC Bootloader](https://www.microchip.com/16-bit-bootloader)
* [dsPIC33C Touch-CAN-LIN Curiosity Development Board (EV97U97A)](https://www.microchip.com/en-us/development-tool/EV97U97A)

### Setup And Operation
See [secure_boot_and_secure_firmware_upgrade_over_canfd](secure_boot_and_secure_firmware_upgrade_over_canfd/) and the associated [README.md](secure_boot_and_secure_firmware_upgrade_over_canfd/README.md) files for details on the required hardware, software, setup, and how to run the demo. 
