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

#define USER_INPUT_BUFFER_SIZE 50
#define UNLOCK_COMMAND "LOCKDEVICE"
#define MATCHES 0
#define ENTER '\r'

// Function prototypes
static void ClearTerminalScreen(void);
static void ClearTerminalLine(void);
static void MoveCursor(int row);
static void EnableCursor(bool enable);
static void PrintWarning(void);
static uint32_t GetResetAddress();
static bool WasLoadedByBootloader();
static void PrintBootloaderRequired(void);
static char* ScanINPUT(void);
static void DisableProgrammingPort(void);
static void InvalidKeyword(void);
static void PrintProgrammingDisabled(void);

// Local Variables
static char userInput[USER_INPUT_BUFFER_SIZE] = {0};
static bool ICSP_INHIBIT_IsEnabled(void);   //temp stub - replace with real version
static bool ICSP_INHIBIT_Enable();          //temp stub - replace with real version
static bool errorPresent = false;
static bool icspEnabled = false;


int main(void)
{
    SYSTEM_Initialize();
        
//    if(WasLoadedByBootloader() == false)
//    {
//        PrintBootloaderRequired();
//        
//        while(1)
//        {
//        }
//    }
//    else
    {
        const char* keyword = "LOCKDEVICE";
        
        PrintWarning();

        while (1)
        {
            char* userInput;
                     
            if(ICSP_INHIBIT_IsEnabled())
            {
                PrintProgrammingDisabled();
                
                while(1)
                {
                }
            }
            
            userInput = ScanINPUT();
            
            if(strcmp(userInput, keyword) == MATCHES)
            {
                DisableProgrammingPort();
            }
            else
            {
                InvalidKeyword();
            }
        }
    }
}

static void PrintProgrammingDisabled(void)
{
    EnableCursor(false);
    MoveCursor(1);
    ClearTerminalScreen();
            
    printf("\r\n");
    printf("\r\n");
    printf("\r\n");
    printf("*** ICSP Programming/Debugging permanently disabled. ***\r\n");
    printf("\r\n");
    printf("Use the bootloader to load all future applications into this board.");
}

//temp stub - replace with real version
static bool ICSP_INHIBIT_IsEnabled(void)
{
    return icspEnabled;
}

static bool ICSP_INHIBIT_Enable()
{
    icspEnabled = true;
}

static void InvalidKeyword(void)
{
    MoveCursor(7);
    ClearTerminalLine();
    printf("Invalid keyword entered. Try again.");
    errorPresent = true;
    
    MoveCursor(5);
    ClearTerminalLine();
    printf(">> ");
    
}

static void DisableProgrammingPort(void)
{
    ICSP_INHIBIT_Enable();
}

static char* ScanINPUT(void)
{
    uint8_t offset = 0;
    char key;
    
    memset(userInput, 0, sizeof(userInput));
    
    do
    {     
        key = UART1_Read();
        
        if(errorPresent)
        {
            MoveCursor(7);
            ClearTerminalLine();
            MoveCursor(5);
            printf(">> ");
            errorPresent = false;
        }
        
        if(key != ENTER)
        {
            userInput[offset++] = key;
            printf("%c", key);
        }
    }
    while((key != ENTER) && (offset < sizeof(userInput)));

    return userInput;
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


/*
 * "\033[?25h" for cursor enable
 * "\033[?25l" for cursor disable
 */
static void EnableCursor(bool enable)
{
    printf("\033[?25%c", enable ? 'h' : 'l');
}

static void PrintWarning(void)
{
    EnableCursor(false);
    MoveCursor(1);
    ClearTerminalScreen();
    
    printf("Type LOCKDEVICE (plus ENTER) to enable the ICSP Inhibit feature.\r\n");
    printf("\r\n");
    printf("WARNING: THIS PERMANENTLY DISABLES DIRECT PROGRAMMING OF THE BOARD.\r\n");
    printf("\r\n");
    printf(">> ");
    
    EnableCursor(true);
}

static void PrintBootloaderRequired(void)
{
    EnableCursor(false);
    MoveCursor(1);
    ClearTerminalScreen();
    
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