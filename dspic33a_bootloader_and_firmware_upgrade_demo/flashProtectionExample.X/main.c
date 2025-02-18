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

#include "bsp/led7.h"
#include "bsp/led6.h"
#include "bsp/led5.h"

#include "bsp/s1.h"

#include "mdfu/mdfu_boot_entry.h"

#include "mcc_generated_files/flash/flash.h"

/*
    Main application

    This demo attempts to unlock and erase parts of the boot flash and application flash areas.
    Both of these memory areas should be write protected and locked until reset coming out
    of the boot loader to prevent self writes to the device.
*/

static volatile bool bootCorrupted = false;
static volatile bool appCorrupted = false;

/* This is here to put something in the bootloader space in case this is
 * programmed rather than boot loaded so we have something to test.
 */
#define BOOT_TEST_CODE_ADDRESS 0x801000
static const volatile uint32_t bootTestCode __attribute__((address(BOOT_TEST_CODE_ADDRESS), space(prog))) = 0x01234567UL;

#define APP_TEST_CODE_ADDRESS 0x80D000
static const volatile uint32_t appTestCode __attribute__((address(APP_TEST_CODE_ADDRESS), space(prog))) = 0x76543210UL;

#define FLASH_PROTECTION_KEY 0xB7370000UL
#define FLASH_PROTECTION_UNLOCKED 0x00000003UL
#define FLASH_PROTECTION_LOCKED_UNTIL_RESET 0x00000001UL
#define FLASH_PROTECTION_LOCKED_BUT_UNLOCKABLE 0x00000000UL

#define BLANK_INSTRUCTION 0xFFFFFFFFUL

/**
  @brief    Attempts to unlock the boot flash region and erase a page of data.
 */
static bool BootCorrupt(void)
{
    bool corruptionDetected = false;

    uint32_t physicalEraseAddress = FLASH_ErasePageAddressGet(BOOT_TEST_CODE_ADDRESS);

    /* Attempt to unlock the bootloader flash protection region. */
    PR0LOCK = FLASH_PROTECTION_KEY | FLASH_PROTECTION_UNLOCKED;

    /* And make the boot flash pages writable. */
    PR0CTRLbits.WR = 1;

    (void)FLASH_PageErase(physicalEraseAddress, FLASH_UNLOCK_KEY);

    if (bootTestCode == BLANK_INSTRUCTION)
    {
        corruptionDetected = true;
    }

    return corruptionDetected;
}

/**
  @brief    Attempts to unlock the application flash region and erase a page of data.
 */
static bool AppCorrupt(void)
{
    bool corruptionDetected = false;

    uint32_t physicalEraseAddress = FLASH_ErasePageAddressGet(BOOT_TEST_CODE_ADDRESS);

    /* Attempt to unlock the bootloader flash protection region. */
    PR1LOCK = FLASH_PROTECTION_KEY | FLASH_PROTECTION_UNLOCKED;

    /* And make the boot flash pages writable. */
    PR1CTRLbits.WR = 1;

    (void)FLASH_PageErase(physicalEraseAddress, FLASH_UNLOCK_KEY);

    if (bootTestCode == BLANK_INSTRUCTION)
    {
        corruptionDetected = true;
    }

    return corruptionDetected;
}

/**
  @brief    Blink LEDs based on a timer.
            - LED7 is keep alive/always.
            - LED6 indicates that the erase to boot flash was allowed.
            - LED 5 indicates that the erase to application flash was allowed.
  */
void TMR1_TimeoutCallback()
{
    LED7_Toggle();

    if (bootCorrupted)
    {
        LED6_Toggle();
    }

    if (appCorrupted)
    {
        LED5_Toggle();
    }
}

int main(void)
{
    SYSTEM_Initialize();

    LED7_Initialize();
    LED6_Initialize();
    LED5_Initialize();

    LED7_On();
    LED6_On();
    LED5_On();

    BUTTON_S1_Initialize();

    bootCorrupted = BootCorrupt();
    appCorrupted = AppCorrupt();

    while (1)
    {
        if (BUTTON_S1_IsPressed())
        {
            MDFU_BootEntry();
        }
    }
}