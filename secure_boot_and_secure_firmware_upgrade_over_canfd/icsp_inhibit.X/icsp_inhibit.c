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

#include <stdbool.h>
#include "mcc_generated_files/flash/flash.h"
#include "mcc_generated_files/flash/flash_types.h"

static flash_data_t inhibitKeys[4] = 
{
    0x00006D63UL, 0x00000000UL, 0x00006870UL, 0x00000000UL
};

bool ICSP_INHIBIT_IsEnabled(void)
{
    static flash_data_t dataInFlash[4];
    FLASH_Read(0x801034, 4, dataInFlash);
    
    if(memcmp(inhibitKeys, dataInFlash, sizeof(inhibitKeys)) == 0)
    {
        return true;
    }
    return false;
}

void ICSP_INHIBIT_Enable()
{
    FLASH_WordWrite(0x801034, &inhibitKeys[0], FLASH_UNLOCK_KEY);
    FLASH_WordWrite(0x801038, &inhibitKeys[2], FLASH_UNLOCK_KEY); 
}