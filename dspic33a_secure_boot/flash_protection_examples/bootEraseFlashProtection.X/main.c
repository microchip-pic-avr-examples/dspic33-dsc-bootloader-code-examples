/*
� [2025] Microchip Technology Inc. and its subsidiaries.

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
#include <stdbool.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/system/pins.h"
#include "mcc_generated_files/timer/tmr1.h"
#include "mcc_generated_files/system/traps.h"
#include "mcc_generated_files/flash/flash.h"

#include "bsp/led7.h"
#include "bsp/led6.h"
#include "bsp/led2.h"
#include "bsp/s3.h"

#include "mdfu/mdfu_boot_entry.h"

#include "flash_protection.h"

/*
    Main application

    This demo attempts to unlock and erase part of the boot flash area.
    This memory area should be write protected and locked until reset coming out
    of the boot loader to prevent self writes to the device. 

    If the FIRT (IRT enabled) and IRT DONE bits are set in the bootloader, 
    the erase attempt will trigger a CPU X Data Bus Error Trap.
*/
#define BYTES_PER_INSTRUCTION 4U
#define FLASH_ERASE_PAGE_SIZE_IN_BYTE (FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS * BYTES_PER_INSTRUCTION)

static volatile bool bootErased = false;
static volatile bool irtAccessed = true;

/* Define a section in memory to indicate if we successfully attacked the memory 
 * or not. We put a defined value here so that if we program this demo directly,
 * we can see if the memory is successfully attacked or not.
 * 
 * We create a second section to make sure that no code is allocated to the page
 * that will be erased in the attempted "attack".
 */

/* Define a test section in the BOOT area of memory. */
#define BOOT_TEST_CODE_ADDRESS 0x801000
static const volatile uint8_t bootTestCodeBuffer[FLASH_ERASE_PAGE_SIZE_IN_BYTE - sizeof(uint32_t)] __attribute__((address(BOOT_TEST_CODE_ADDRESS+sizeof(uint32_t)), space(prog), keep)) = {0};

#define FLASH_PROTECTION_KEY 0xB7370000UL
#define FLASH_PROTECTION_UNLOCKED 0x00000003UL
#define FLASH_PROTECTION_LOCKED_UNTIL_RESET 0x00000001UL
#define FLASH_PROTECTION_LOCKED_BUT_UNLOCKABLE 0x00000000UL

#define BLANK_INSTRUCTION 0xFFFFFFFFUL

/**
  @brief    Attempts to unlock the boot flash region and erase a page of data.
 */
static bool BootErase(void)
{
    static const volatile uint32_t bootTestCode __attribute__((address(BOOT_TEST_CODE_ADDRESS), space(prog))) = 0x01234567UL;
    bool eraseDetected = false;

    uint32_t physicalEraseAddress = FLASH_ErasePageAddressGet(BOOT_TEST_CODE_ADDRESS);

    /* Attempt to unlock the bootloader flash protection region. */
    PR0LOCK = FLASH_PROTECTION_KEY | FLASH_PROTECTION_UNLOCKED;

    /* And make the boot flash pages writable. */
    PR0CTRLbits.WR = 1;

    (void)FLASH_PageErase(physicalEraseAddress, FLASH_UNLOCK_KEY);

    if (bootTestCode == BLANK_INSTRUCTION)
    {
        eraseDetected = true;
    }

    return eraseDetected;
}

/**
  @brief    Blink LEDs based on a timer.
            - LED7 is keep alive/always.
            - LED6 indicates that the erase to boot flash was allowed.
            - LED2 indicates that a region of IRT memory was accessed and a Bus
              Error Trap failed to trigger.
  */
void TMR1_TimeoutCallback(void)
{
    LED7_Toggle();

    if (bootErased)
    {
        LED6_Toggle();
    }
    
    if (irtAccessed)
    {
        LED2_Toggle();
    }
}

/**
  @brief    Bus trap error handler. This trap will trigger when an attempt is 
 *          made to access an IRT region when FIRT is enabled in the bootloader 
 *          and the IRT DONE bit has been set. 
 */
void ERROR_HANDLER _BusErrorTrap(void)
{
    irtAccessed = false;
    INTCON3bits.XRAMBET = 0;  //Clear the trap flag
}

int main(void)
{
    SYSTEM_Initialize();

    LED7_Initialize();
    LED6_Initialize();
    LED2_Initialize();

    LED7_On();
    LED6_On();
    LED2_On();

    BUTTON_S3_Initialize();

    bootErased = BootErase();

    while (1)
    {
        if (BUTTON_S3_IsPressed())
        {
            MDFU_BootEntry();
        }
    }
}