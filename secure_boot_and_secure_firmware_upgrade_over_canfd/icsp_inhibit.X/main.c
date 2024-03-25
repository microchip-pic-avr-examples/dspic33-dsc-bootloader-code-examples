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

#include "mcc_generated_files/flash/flash.h"
#include "mcc_generated_files/flash/flash_types.h"
#include "mcc_generated_files/boot/boot_config.h"
#include "mcc_generated_files/uart/uart1.h"
#include "mcc_generated_files/system/system.h"
#include "icsp_inhibit.h"
#include "terminal.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define USER_INPUT_BUFFER_SIZE 50
#define UNLOCK_COMMAND "LOCKDEVICE"
#define STRCMP_MATCHES 0
#define ENTER '\r'
#define TERMINAL_LINE_ERROR 7
#define TERMINAL_LINE_INPUT 5
#define TERMINAL_LINE_HOME 1

// Function prototypes
static void PrintWarning(void);
static uint32_t GetResetAddress();
static bool WasLoadedByBootloader();
static void PrintBootloaderRequired(void);
static char* ScanInput(void);
static void PrintErrorMessage(char* error);
static void PrintProgrammingDisabled(void);
static void ClearUserInput(void);
static void ClearErrorMessage(void);
static void ResetPrompt(void);

// Local Variables
static char userInput[USER_INPUT_BUFFER_SIZE] = {0};
static bool errorPresent = false;

int main(void)
{
    const char* keyword = "LOCKDEVICE";

    SYSTEM_Initialize();
    
    if(WasLoadedByBootloader() == false)
    {
        PrintBootloaderRequired();
        
        while(1)
        {
        }
    }
    
    if(ICSP_INHIBIT_IsEnabled())
    {
        PrintProgrammingDisabled();

        while(1)
        {
        }
    }
            
    PrintWarning();

    while(1)
    {
        char* userInput = ScanInput();

        if(strcmp(userInput, keyword) == STRCMP_MATCHES)
        {
            //ICSP_INHIBIT_Enable();        //Disable during development for safety

            PrintProgrammingDisabled();

            while(1)
            {
            }
        }
        else
        {
            PrintErrorMessage("Invalid keyword entered. Try again.");
        }
    }
}

static char* ScanInput(void)
{
    uint8_t userInputOffset = 0;
    char key;
    
    ClearUserInput();
    
    do
    {     
        key = UART1_Read();
        
        /* If there is still an error message after the first user key press,
         * clear the error and reset the prompt before printing their input. */
        if(errorPresent)
        {
            ClearErrorMessage();
            ResetPrompt();
        }
        
        if(isalnum(key) && (userInputOffset < sizeof(userInput)))
        {
            userInput[userInputOffset++] = key;
            printf("%c", key);
        }
    }
    while(key != ENTER);

    return userInput;
}

static void PrintProgrammingDisabled(void)
{
    TERMINAL_EnableCursor(false);
    TERMINAL_MoveCursor(TERMINAL_LINE_HOME);
    TERMINAL_ClearScreen();
            
    printf("\r\n");
    printf("\r\n");
    printf("\r\n");
    printf("*** ICSP Programming/Debugging permanently disabled. ***\r\n");
    printf("\r\n");
    printf("Use the bootloader to load all future applications into this board.");
}

static void ClearErrorMessage(void)
{
    TERMINAL_MoveCursor(TERMINAL_LINE_ERROR);
    TERMINAL_ClearLine();
        
    errorPresent = false;
}

static void ResetPrompt(void)
{
    TERMINAL_MoveCursor(TERMINAL_LINE_INPUT);
    TERMINAL_ClearLine();
    printf(">> ");
}

static void PrintErrorMessage(char* errorMessage)
{
    ClearErrorMessage();
    
    printf("%s", errorMessage);
    errorPresent = true;
    
    ResetPrompt();
}

static void ClearUserInput(void)
{
    memset(userInput, 0, sizeof(userInput));
}

static void PrintWarning(void)
{
    TERMINAL_EnableCursor(false);
    TERMINAL_MoveCursor(TERMINAL_LINE_HOME);
    TERMINAL_ClearScreen();
    
    printf("Type LOCKDEVICE (plus ENTER) to enable the ICSP Inhibit feature.\r\n");
    printf("\r\n");
    printf("WARNING: THIS PERMANENTLY DISABLES DIRECT PROGRAMMING OF THE BOARD.\r\n");
    printf("\r\n");
    printf(">> ");
    
    TERMINAL_EnableCursor(true);
}

static void PrintBootloaderRequired(void)
{
    TERMINAL_EnableCursor(false);
    TERMINAL_MoveCursor(TERMINAL_LINE_HOME);
    TERMINAL_ClearScreen();
    
    printf("NO BOOTLOADER DETECTED!\r\n");
    printf("\r\n");
    printf("Because programming will be permanently disabled, \r\n");
    printf("a bootloader is required to run this demo. \r\n");
    printf("Please see the readme.md for more information.\r\n");
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