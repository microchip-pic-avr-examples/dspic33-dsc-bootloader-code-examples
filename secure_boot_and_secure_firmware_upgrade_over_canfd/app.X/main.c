/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
 */
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/system/pins.h"
#include "mcc_generated_files/flash/flash.h"
#include "mcc_generated_files/flash/flash_types.h"
#include "mcc_generated_files/boot/boot_config.h"

/*
    Main application
 */

/* The following code finds the address used by GOTO instruction programmed 
 * at the reset vector in order to determine whether or not the application 
 * was downloaded directly or via the bootloader (i.e. if the address within the 
 * GOTO instruction is within the bootloader or application), as the ICSP 
 * inhibit functionality is only permitted if the application was 
 * downloaded via the bootloader.
 * 
 * The GOTO instruction contains two 24 bit words, the first of which contains 
 * the lower 15 bits of the address  and the second of which contains the upper 
 * 7 bits. The following code reads the GOTO address at the reset vector and 
 * parses the two words in order to determine the reset address. The opcode bits
 * in the GOTO instruction are masked. 
 * 
 * Refer to https://ww1.microchip.com/downloads/en/DeviceDoc/70000157g.pdf for 
 * additional details on the GOTO instruction format.   
 */
uint32_t GetResetAddress()
{   
    flash_data_t flashData[2];
    FLASH_Read(0x000000, 2, flashData);
    return (((flashData[1] & 0x0000007F)<<16) | (flashData[0] & 0x0000FFFE));
}

bool WasLoadedByBootloader()
{
    return (GetResetAddress() < BOOT_CONFIG_PROGRAMMABLE_ADDRESS_LOW);
}

int main(void) {

    SYSTEM_Initialize();
    
    while (1) 
    {
        if (WasLoadedByBootloader()) 
        {
            IO_RE6_SetHigh();
        }
    }
}