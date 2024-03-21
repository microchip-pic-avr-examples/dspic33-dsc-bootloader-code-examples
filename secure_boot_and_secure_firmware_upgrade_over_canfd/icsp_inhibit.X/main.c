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

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#warning "!!RUNNING THIS PROGRAM AND FOLLOWING THE STEPS OUTLINED IN THE CONSOLE WILL PERMANENTLY DISABLE DIRECT PROGRAMMING OF THE BOARD. FOR ADDITIONAL INFORMATION, SEE THE README.MD INCLUDED WITH THIS PROJECT AND THE FAMILY DATA SHEET LOCATED AT https://ww1.microchip.com/downloads/aemDocuments/documents/MCU16/ProductDocuments/DataSheets/dsPIC33CK1024MP710-Family-Data-Sheet-DS70005496.pdf"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mcc_generated_files/flash/flash.h"
#include "mcc_generated_files/flash/flash_types.h"
#include "mcc_generated_files/boot/boot_config.h"

#define WINDOW_SIZE 10
#define UNLOCK_COMMAND "LOCKDEVICE"

// Function prototypes
static void ClearTerminalScreen(void);
static void ClearTerminalLine(void);
static void MoveCursor(int row);
static void HideCursor(void);
static void PrintWarning(void);
static void AppendCharToWindow(char receivedChar, char *window, int *windowIndex);
static void ProcessReceivedChar(char receivedChar, char *window, int *windowIndex);
static void ResetWindowOnMismatch(char *window, int *windowIndex);
static void CheckForUnlockCommand(char *window, int *windowIndex);
static uint32_t GetResetAddress();
static bool WasLoadedByBootloader();
static void PrintBootloaderRequired(void);

int main(void)
{
    char window[WINDOW_SIZE + 1] = {0};
    int windowIndex = 0;

    SYSTEM_Initialize();
    HideCursor();
    ClearTerminalScreen();
    
    if(WasLoadedByBootloader() == false)
    {
        PrintBootloaderRequired();
        
        while(1)
        {
        }
    }
    else
    {
        PrintWarning();

        while (1)
        {
            if (UART1_IsRxReady())
            {
                char receivedChar = UART1_Read();
                ProcessReceivedChar(receivedChar, window, &windowIndex);
                CheckForUnlockCommand(window, &windowIndex);
            }
        }
    }
}

static void ClearTerminalScreen(void)
{
    printf("\033[2J");
}

static void ClearTerminalLine(void)
{
    printf("\33[2K\r");
}

static void MoveCursor(int row)
{
    printf("\033[%d;0f", row);
}

static void HideCursor(void)
{
    printf("\033[?25l");
}

static void PrintWarning(void)
{
    MoveCursor(1);
    printf("Type LOCKDEVICE to enable the ICSP Inhibit feature.\r\n\r\n");
    printf("WARNING: THIS PERMANENTLY DISABLES DIRECT PROGRAMMING OF THE BOARD.");
}

static void PrintBootloaderRequired(void)
{
    MoveCursor(1);
    printf("NO BOOTLOADER DETECTED!\r\n\r\n");
    printf("Because programming will be permanently disabled, \r\na bootloader is required to run this demo. \r\nPlease see the readme.md for more information.");
}

static void ProcessReceivedChar(char receivedChar, char *window, int *windowIndex)
{
    bool isCharValid = (*windowIndex < strlen(UNLOCK_COMMAND)) && (receivedChar == UNLOCK_COMMAND[*windowIndex]);
    
    if (isCharValid)
    {
        AppendCharToWindow(receivedChar, window, windowIndex);
        MoveCursor(10);
        printf("%s", window);
    }
    else
    {
        ResetWindowOnMismatch(window, windowIndex);
    }
}

static void AppendCharToWindow(char receivedChar, char *window, int *windowIndex)
{
    window[(*windowIndex)++] = receivedChar;
    window[*windowIndex] = '\0';
}

static void ResetWindowOnMismatch(char *window, int *windowIndex)
{
    MoveCursor(10);
    ClearTerminalLine();
    MoveCursor(5);
    ClearTerminalLine();
    printf("Invalid character entered. Try again.");
    *windowIndex = 0;
    memset(window, 0, WINDOW_SIZE + 1); 
}

static void CheckForUnlockCommand(char *window, int *windowIndex)
{
    if (strncmp(window, UNLOCK_COMMAND, *windowIndex) == 0 && *windowIndex == strlen(UNLOCK_COMMAND))
    {
        MoveCursor(5);
        ClearTerminalLine();
        printf("ICSP Programming/Debugging permanently disabled. \n");
        MoveCursor(10);
        ClearTerminalLine();
        *windowIndex = 0;
        memset(window, 0, WINDOW_SIZE + 1);
    }
}

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
static uint32_t GetResetAddress()
{   
    flash_data_t flashData[2];
    FLASH_Read(0x000000, 2, flashData);
    return (((flashData[1] & 0x0000007F)<<16) | (flashData[0] & 0x0000FFFE));
}

static bool WasLoadedByBootloader()
{
    return (GetResetAddress() < BOOT_CONFIG_PROGRAMMABLE_ADDRESS_LOW);
}