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
#define UNLOCK_COMMAND "LOCKDEVICE"

static void clearTerminalScreen(void)
{
    printf("\033[2J"); 
}

static void clearTerminalLine(void)
{
    printf("\33[2K\r");
}

static void moveCursor(int row)
{
    printf("\033[%d;0f", row);
}

static void hideCursor()
{
    printf("\033[?25l");
}

static void printWarning()
{
    moveCursor(1);
    printf("Enter LOCKDEVICE to unlock the ICSP Inhibit feature.");
    moveCursor(3);
    printf("WARNING: THIS PERMANENTLY DISABLES DIRECT PROGRAMMING OF THE BOARD.");
}

int main(void)
{
    char window[WINDOW_SIZE + 1];
    int windowIndex = 0;
    int patternLength = strlen(UNLOCK_COMMAND);
    SYSTEM_Initialize();
    clearTerminalScreen();
    printWarning();
    while(1)
    {    
        if(UART1_IsRxReady())
        {
            char receivedChar = UART1_Read();
            
            if(receivedChar == UNLOCK_COMMAND[windowIndex])
            {
                window[windowIndex++] = receivedChar; 
                window[windowIndex] = '\0'; 
            
                if(windowIndex == patternLength)
                {
                    moveCursor(5);
                    clearTerminalLine();
                    printf("Unlock command for ICSP Inhibit received. \n");
                    hideCursor();
                    windowIndex = 0;
                }
            }
            else
            {
                moveCursor(5);
                clearTerminalLine();
                printf("Incorrect command received. Try again.");
                windowIndex = (receivedChar == UNLOCK_COMMAND[0]) ? 1 : 0;
                window[0] = (windowIndex == 1) ? receivedChar : '\0';
            }
            while(!UART1_IsTxReady())
            {
            }    
        }
    }
}    