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

    This example attempts to read an instruction from the keystore flash area. 
    Although the keystore flash region has reads enabled, IRT sections are 
    unreadable from non-IRT sections, therefore the read attempt should be 
    blocked. 
 
    If the FIRT (IRT enabled) and IRT DONE bits are set in the bootloader, then 
    the read attempt will trigger a CPU X Data Bus Error Trap.
*/

#define BYTES_PER_INSTRUCTION 4U
#define FLASH_ERASE_PAGE_SIZE_IN_BYTE (FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS * BYTES_PER_INSTRUCTION)

static volatile bool keystoreRead = false;
static volatile bool irtAccessed = true;

#define KEYSTORE_INSTRUCTION 0x01234567UL

/* Define a test section in the KEYSTORE area of memory to verify the read. */
#define KEYSTORE_TEST_CODE_ADDRESS 0x80A000
static const volatile uint32_t keystoreTestCode __attribute__((address(KEYSTORE_TEST_CODE_ADDRESS), space(prog))) = KEYSTORE_INSTRUCTION;

/**
  @brief    Attempts to read data at a specific location in the keystore flash 
 *          region.
 */
static bool KeystoreRead(void)
{
    bool readDetected = false;
    uint32_t keystoreData;

    (void)FLASH_Read(KEYSTORE_TEST_CODE_ADDRESS, 1, &keystoreData);

    if (keystoreData == KEYSTORE_INSTRUCTION)
    {
        readDetected = true;
    }

    return readDetected;
}

/**
  @brief    Blink LEDs based on a timer.
            - LED7 is keep alive/always.
            - LED6 indicates that the read of keystore flash was allowed.
            - LED2 indicates that a region of IRT memory was accessed and a Bus
              Error Trap failed to trigger.
  */
void TMR1_TimeoutCallback(void)
{
    LED7_Toggle();

    if (keystoreRead)
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

    keystoreRead = KeystoreRead();

    while (1)
    {
        if (BUTTON_S3_IsPressed())
        {
            MDFU_BootEntry();
        }
    }
}