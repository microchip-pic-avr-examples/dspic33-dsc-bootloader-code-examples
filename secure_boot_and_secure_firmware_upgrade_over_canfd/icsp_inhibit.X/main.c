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
#include "mcc_generated_files/uart/uart1.h"
#include "mcc_generated_files/system/pins.h"
#include <stdio.h>
#include <string.h>

#define WINDOW_SIZE 10
#define UNLOCK_PATTERN "LOCKDEVICE"

int main(void)
{
    char window[WINDOW_SIZE + 1];
    int windowIndex = 0;
    int patternLength = strlen(UNLOCK_PATTERN);
    SYSTEM_Initialize();
    printf("\033[2J"); // Clear the screen
    
    while(1)
    {    
        if(UART1_IsRxReady())
        {
            char receivedChar = UART1_Read();
            
            
            if(receivedChar == UNLOCK_PATTERN[windowIndex])
            {
                window[windowIndex++] = receivedChar; 
                window[windowIndex] = '\0'; 
            
                
                if(windowIndex == patternLength)
                {
                    printf("PATTERN FOUND! \n");
                    windowIndex = 0;
                }
            }
            else
            {
                windowIndex = (receivedChar == UNLOCK_PATTERN[0]) ? 1 : 0;
                window[0] = (windowIndex == 1) ? receivedChar : '\0';
            }
            
            while(!UART1_IsTxReady())
            {
            }
            UART1_Write(receivedChar);
        }
    }
}    