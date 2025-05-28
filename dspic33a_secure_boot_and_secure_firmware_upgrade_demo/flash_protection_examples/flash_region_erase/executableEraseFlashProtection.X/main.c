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

#include "bsp/led7.h"
#include "bsp/led6.h"
#include "bsp/led2.h"

#include "bsp/s3.h"

#include "mdfu/mdfu_boot_entry.h"

#include "mcc_generated_files/flash/flash.h"

/*
    Main application

    This demo attempts to unlock and erase part of the executable flash area.
    This memory area should be write protected and locked until reset coming out
    of the boot loader to prevent self writes to the device. 
*/
#define BYTES_PER_INSTRUCTION 4U
#define FLASH_ERASE_PAGE_SIZE_IN_BYTE (FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS * BYTES_PER_INSTRUCTION)

static volatile bool executableErased = false;

/* Define a section in memory to indicate if we successfully attacked the memory 
 * or not. We put a defined value here so that if we program this demo directly,
 * we can see if the memory is successfully attacked or not.
 * 
 * We create a second section to make sure that no code is allocated to the page
 * that will be erased in the attempted "attack".
 */

/* Define a test section in the EXECUTABLE area of memory. */
#define EXECUTABLE_TEST_CODE_ADDRESS 0x844000
static const volatile uint8_t executableTestCodeBuffer[FLASH_ERASE_PAGE_SIZE_IN_BYTE - sizeof(uint32_t)] __attribute__((address(EXECUTABLE_TEST_CODE_ADDRESS+sizeof(uint32_t)), space(prog), keep)) = {0};

#define FLASH_PROTECTION_KEY 0xB7370000UL
#define FLASH_PROTECTION_UNLOCKED 0x00000003UL
#define FLASH_PROTECTION_LOCKED_UNTIL_RESET 0x00000001UL
#define FLASH_PROTECTION_LOCKED_BUT_UNLOCKABLE 0x00000000UL

#define BLANK_INSTRUCTION 0xFFFFFFFFUL

/**
  @brief    Attempts to unlock the executable flash region and erase a page of data.
 */
static bool ExecutableErased(void)
{
    static const volatile uint32_t executableTestCode __attribute__((address(EXECUTABLE_TEST_CODE_ADDRESS), space(prog))) = 0x76543210UL;
    bool eraseDetected = false;

    uint32_t physicalEraseAddress = FLASH_ErasePageAddressGet(EXECUTABLE_TEST_CODE_ADDRESS);

    /* Attempt to unlock the application flash protection region. */
    PR2LOCK = FLASH_PROTECTION_KEY | FLASH_PROTECTION_UNLOCKED;

    /* And make the application flash pages writable. */
    PR2CTRLbits.WR = 1;

    (void)FLASH_PageErase(physicalEraseAddress, FLASH_UNLOCK_KEY);

    if (executableTestCode == BLANK_INSTRUCTION)
    {
        eraseDetected = true;
    }

    return eraseDetected;
}

/**
  @brief    Blink LEDs based on a timer.
            - LED7 is keep alive/always.
            - LED6 indicates that the erase to executable flash was allowed.
  */
void TMR1_TimeoutCallback(void)
{
    LED7_Toggle();

    if (executableErased)
    {
        LED6_Toggle();
    }
}

int main(void)
{
    SYSTEM_Initialize();

    LED7_Initialize();
    LED6_Initialize();

    LED7_On();
    LED6_On();

    BUTTON_S3_Initialize();

    executableErased = ExecutableErased();

    while (1)
    {
        if (BUTTON_S3_IsPressed())
        {
            MDFU_BootEntry();
        }
    }
}