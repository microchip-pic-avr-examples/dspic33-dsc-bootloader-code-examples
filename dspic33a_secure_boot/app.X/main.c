/*
? [2025] Microchip Technology Inc. and its subsidiaries.

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
#include "mcc_generated_files/timer/tmr1.h"

#include "bsp/led7.h"
#include "bsp/led5.h"

#include "bsp/s3.h"

#include "mdfu/mdfu_boot_entry.h"

#include "mdfu/mdfu_config.h"

/*
    Main application
*/

void TMR1_TimeoutCallback(void)
{
    LED7_Toggle();
}

#define CPU_DELAY 0x200000

int main(void)
{
    uint32_t delay = 0;
    
    //Enable cache - was disabled by the boot loader 
    CHECONbits.ON = 1;  
    
    SYSTEM_Initialize();

    LED7_Initialize();
    LED5_Initialize();
    
    BUTTON_S3_Initialize();
    
    while(1)
    {
        if(++delay >= CPU_DELAY)
        {
            delay = 0;
            LED5_Toggle();
        }
        
        if(BUTTON_S3_IsPressed())
        {
            MDFU_BootEntry();
        }
    }    
}