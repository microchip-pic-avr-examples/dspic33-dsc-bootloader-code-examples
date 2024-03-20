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
 
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

int main(void)
{
    char window[WINDOW_SIZE + 1] = {0};
    int windowIndex = 0;

    SYSTEM_Initialize();
    ClearTerminalScreen();
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
    printf("Type LOCKDEVICE to enable the ICSP Inhibit feature.");
    MoveCursor(3);
    printf("WARNING: THIS PERMANENTLY DISABLES DIRECT PROGRAMMING OF THE BOARD.");
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
        printf("Unlock command successful.\n");
        MoveCursor(10);
        ClearTerminalLine();
        *windowIndex = 0;
        memset(window, 0, WINDOW_SIZE + 1);
    }
}