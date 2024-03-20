<picture>
    <source media="(prefers-color-scheme: dark)" srcset="../images/microchip_logo_white_red.png">
	<source media="(prefers-color-scheme: light)" srcset="../images/microchip_logo_black_red.png">
    <img alt="Microchip Logo." src="../images/microchip_logo_black_red.png">
</picture>

## Secure Boot and Secure Firmware Upgrade over CAN-FD for dsPIC33C DSCs Demo

![Board picture](./images/board.jpg)

## Introduction
The associated bootloader and application projects demonstrate the following: 
* Immutable secure boot via CodeGuard
* A/B type bootloading with Flash partitioning to support two firmware images and anti-roll back
* Secure boot authentication via ECDSA signature verification 
* Secure firmware update via CAN-FD
* Crypto acceleration via TA100

## Related Documentation
* [dsPIC33C Touch-CAN-LIN Curiosity Development Board User's Guide](https://www.microchip.com/en-us/development-tool/EV97U97A)
* [dsPIC33CK1024MP710 Datasheet](https://www.microchip.com/en-us/product/dsPIC33CK1024MP710) 

## Tools 

#### Software 
* **THIS PROJECT REQUIRES A NON PUBLIC COPY OF THE CRYPTOGRAPHIC AUTHENTICATION LIBRARY (CAL) FOR TA100 SUPPORT! See the [CryptoAuthentication Library Release Notes](https://onlinedocs.microchip.com/pr/GUID-7F2639F3-1541-4BFC-A031-9A718BFFC502-en-US-16/index.html?GUID-7D69101E-EA71-47BE-B794-7F2CA6363B2C) for download details**
* The unzipped example project files, boot.X and app.X
* [MPLAB® X IDE v6.15 or later](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide)
* [MPLAB® XC16 v2.10 or later](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers)
* [Universal Bootloader Host Application v1.19.3 or later](https://www.microchip.com/16-bit-bootloader) 
* Device Family Pack 1.12.354
* [MPLAB® Code Configurator (MCC) v5.6.1 or later (optional - for code configuration)](https://www.microchip.com/en-us/tools-resources/configure/mplab-code-configurator)
* 16-Bit Bootloader MCC module v1.25.0 or later (optional - for code configuration)
* The drivers for the selected Peak CAN-FD protocol analyzer/protocol generator (available at https://www.peak-system.com/Home.59.0.html?&L=1)


#### Hardware
* [dsPIC33C Touch-CAN-LIN Curiosity Development Board (EV97U97A)](https://www.microchip.com/en-us/development-tool/EV97U97A)
* A Peak CAN protocol analyzer/protocol generator. A PCAN-USB FD was used for development of this demo (https://www.peak-system.com/PCAN-USB-FD.365.0.html?&L=1)
* A CAN cable
* A CAN-FD bus terminator (or modify the Peak analyzer per their user's guide for proper CAN-FD termination)
* A micro USB cable, a USB-Type C cable, or a 9v power adapter for the dsPIC33C Touch CAN LIN Curiosity Development Board (for powering the board)
* [TA100 Trust Anchor External HSM 8-Pin SOIC](https://www.microchip.com/en-us/product/ta100) - NOTE: make sure to get the **SPI** variant for this demo.
* [TA100 8-Pin SOIC CryptoAutomotive™ Socket Board](https://www.microchip.com/en-us/development-tool/AC164167)

## Running the Demo

##### Hardware Setup
1. Insert the TA100 into the TA100 8-Pin SOIC CryptoAutomotive™ Socket board.  **NOTE** - make sure to align the TA100 pin 1 indicator with the pin 1 indicator on the socket board silkscreen.<br>
![TA100 Socket Alignment](./images/socket_pin_1.png)
2. Short the J10 jumper to the "3.3v" option.<br>
![Power Jumper](./images/socket_power_jumper.png)
3. Switch all the jumpers on the TA100 8-Pin SOIC CryptoAutomotive™ Socket board to the SPI setting (J3, J4, J5, J6, J7).<br>
![SPI Jumpers](./images/socket_spi_jumper.png)
4. With the Development Board detached from any power source, programmer, or USB connection, connect the TA100 8-Pin SOIC CryptoAutomotive™ Socket board into the mikroBUS™ A slot with the jumpers set for SPI.
5. Connect the Peak analyzer/generator to your computer.
6. Connect the Peak analyzer/generator to the CAN/CAN-FD Driver Circuit on the Development Board.
7. Connect the micro-USB cable to port J1 of the Development Board to the host computer.<br>

![Hardware Setup](./images/hardware_setup.png)

#### Programming the Bootloader
1. Open the boot.X project in MPLAB® X
2. Open MCC
3. Right click on Project Resources and select "Force Update on All"<br>
![Force Update](./images/MCC_Force_Update.png)
4. Click "Generate"<br>
![Generate](./images/MCC_Generate.png)
5. Accept all incoming code changes for files **within the CryptoAuthenticationLibrary directory** by selecting "Replace All" in the merge helper UI<br> **NOTE: DO NOT accept incoming changes for boot_demo.c. Accept all other file changes and close out of MCC**<br>
![Merge Resolution](./images/MCC_Merge_Resolution.png)
6. Press the “Make and Program” button on the top bar<br>
![Make and Program Device](./images/make_and_program.png)
7. Select the appropriate programmer if prompted
8. The project should compile and program successfully
9. Verify the LED11 is solid on the Development Board. This indicates the bootloader is running
    
#### Building the Application (Optional)
**NOTE**: A pre-generated .hex file has been included with this demo, therefore the following steps are **optional**
1. Open the app.X project in MPLAB® X
2. Click the dropdown arrow next to the “Clean and Build Project” button on the top bar and select "Clean and Build Project" <br>**NOTE**: If "Clean and Build for Debugging" is selected instead, a new hex file will not be generated <br>**NOTE**: Make sure not to hit the program button. This will program the application code over the bootloader that was just programmed<br>
![Clean and Build](./images/clean_and_build.png)
3. The project should compile cleanly. app.X/dist/default/production/app.X.production.hex should be generated
4. Verify that LED11 is still solid
    a. If LED11 is blinking instead of solid, then the application code was programmed instead of only compiled. Go back to the “Programming the Bootloader” stage and re-program the bootloader

#### Loading the Application
1. Open the Universal Bootloader Host Application tool (UBHA)<br>
![UBHA](./images/UBHA_open.png)
2. Select the “PIC24/dsPIC” option from the “Device Architecture” selection drop down<br>
![UBHA Device Architecture](./images/UBHA_device_architecture.png)
3. Select “CAN” from the protocol drop down selection box<br>
![UBHA Protocol Selection](./images/UBHA_protocol.png)
4. Select the “Settings->CAN” option from the top menu<br>
![UBHA CAN Settings Dropdown](./images/UBHA_Settings_CAN_Dropdown.png)
5. Select the Peak protocol analyzer being used and the appropriate CAN configuration settings for this demo (listed below). When complete, press “Apply”: 
    a. Nominal Bit Rate: 125.00 kbits/s
    b. CAN-FD: enabled
    c. CAN-FD TX Data Length: 8
    d. Flexible Data Rate: Enabled 
    e. Flexible Data Rate: 2Mbits/s
    f. Message Format: Standard
    g. Host to Device ID: 0xA1
    h. Device to Host ID: 0xA2<br>
![CAN Settings](./images/UBHA_CAN_Settings.png)
6. Press the "Read Device Settings" button
    a. The Application start address and Application end address fields should have updated. If it did not or if you get a communication error, please go back to the “Programming the Bootloader” stage to make sure the bootloader was programmed correctly<br>
![Read Device Settings](./images/UBHA_Read_Device_Settings.png)
7. Load the application hex file by selecting “File->Open/Load File (*.hex)”
    a. Select app.X/dist/default/production/app.X.production.hex. This is included with the demo but may have been re-generated if Building the Application (Optional) was completed<br>
![Open Hex File](./images/UBHA_Open_Hex.png)
8. Check the "Enable Self Verification After Program" checkbox<br>
![Enable Verification](./images/UBHA_Enable_Verification.png)
9. Press “Program Device”. The application should program erase, program and perform a self verify using the TA100 for an ECDSA verify then read back verify correctly.<br>**NOTE**: The reset response may not be fully transmitted before reset which may result in the reset response failing. The reset most likely did occur and the error can be safely ignored 
    a. After a few seconds, LED11 should be blinking<br>
![Program Device](./images/UBHA_Program.png)

#### Re-Entering Bootloader Mode
1. Power off the board by unplugging the micro-USB cable from port J1
2. While the board is powered off, hold down switch S1<br>
![S1](./images/switch_location.png)
3. Continue holding down S1 and plug the micro-USB cable back into port J1
4. Once the board is powered back on, release S1
5. Verify LED11 is solid, indicating the device is in bootloader mode. The application can now be loaded once again (see Loading the Application) 

## Trademarks

MPLAB® is a registered trademark of Microchip Technology Inc. All other trademarks are the property of their respective owner.
