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
#include <stdio.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/system/pins.h"
#include "mcc_generated_files/timer/tmr1.h"
#include "mcc_generated_files/system/traps.h"
#include "mcc_generated_files/flash/flash.h"

#include "bsp/led7.h"
#include "bsp/led2.h"
#include "bsp/s3.h"

#include "flash_protection.h"

#include "mdfu/mdfu_boot_entry.h"
#include "mdfu/mdfu_config.h"

/*
    Main application

    This demo attempts to execute/jump to an area of boot IRT flash memory from 
    a non-IRT region. Although the boot flash region has execution enabled, IRT 
    sections cannot be executed/jumped to from non-IRT sections, therefore the 
    execution should be prevented. 
  
    Given the FIRT (IRT Enable) and IRT DONE bits are set in the bootloader, 
    the execution attempt will trigger a CPU Instruction Data Bus Error Trap. 
*/

// IRT memory space which we try and jump to
#define BOOT_IRT_TEST_CODE_ADDRESS 0x801000

void IRTJump(void)
{   
    void (*irt_space)(void);
    irt_space = (void (*)(void))((uintptr_t)BOOT_IRT_TEST_CODE_ADDRESS);
    irt_space();
}

/**
  @brief    Bus trap error handler. This trap will trigger when an attempt is 
 *          made to access an IRT region when FIRT is enabled in the bootloader
 *          and the IRT DONE bit is set. 
 * 
 *          When triggered, LED2 will turn on.
 */
void ERROR_HANDLER _BusErrorTrap(void)
{
    //Clear the Bus Error trap flags
    INTCON3bits.CPUBET = 0;
    TRAPS_halt_on_error(TRAPS_BUS_ERR);
}

int main(void)
{
    SYSTEM_Initialize();

    LED2_Initialize();
    BUTTON_S3_Initialize();

    IRTJump();

    while (1)
    {
        if (BUTTON_S3_IsPressed())
        {
            MDFU_BootEntry();
        }
    }
}

//@brief Halts 
void TRAPS_halt_on_error(uint16_t code)
{ 
#ifdef __DEBUG    
    /* If we are in debug mode, cause a software breakpoint in the debugger */
    (void)code;
    __builtin_software_breakpoint();
    while(1)
    {
    
    }
#else
    // Turn on LED2 to indicate we triggered a bus error trap 
    if (code == (uint16_t)TRAPS_BUS_ERR)
    {
        LED2_On();
    }
#endif
}
