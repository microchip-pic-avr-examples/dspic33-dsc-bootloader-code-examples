
/**
 * CLOCK Generated Driver Source File 
 * 
 * @file      clock.c
 *            
 * @ingroup   clockdriver
 *            
 * @brief     This is the generated source file for CLOCK driver
 *
 * @version   PLIB Version 1.1.0
 *
 * @skipline  Device : dsPIC33AK128MC106
*/

/*
© [2025] Microchip Technology Inc. and its subsidiaries.

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

// Section: Includes
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../clock.h"
#include "../clock_types.h"

// Section: Static Variables

void CLOCK_Initialize(void)
{
    /*  
        System Clock Source                             :  PLL1 Out output
        System/Generator 1 frequency (Fosc)             :  200 MHz
        
        Clock Generator 2 frequency                     : 8 MHz
        Clock Generator 3 frequency                     : 8 MHz
        
        PLL 1 frequency                                 : 200 MHz
        PLL 1 VCO Out frequency                         : 200 MHz

    */

    /* Always switch to FRC before making clock changes. */
    CLK1CONbits.NOSC = 1; // FRC_CLK
    CLK1CONbits.OSWEN = 1;
    while(CLK1CONbits.OSWEN){}
    
    // NOSC FRC Oscillator; OE enabled; SIDL disabled; ON enabled; BOSC Serial Test Mode clock (PGC); FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL disabled; EXTCFEN disabled; FOUTSWEN disabled; RIS disabled; PLLSWEN disabled; 
    PLL1CON = 0x000129100UL;
    // POSTDIV2 2x divide; POSTDIV1 4x divide; PLLFBDIV 200; PLLPRE 1; 
    PLL1DIV = 0x100C822UL;
    //Enable PLL Input and Feedback Divider update
    PLL1CONbits.PLLSWEN = 1U;
    while (PLL1CONbits.PLLSWEN == 1){};
    PLL1CONbits.FOUTSWEN = 1U;
    while (PLL1CONbits.FOUTSWEN == 1U){};

    //enable clock switching
    PLL1CONbits.OSWEN = 1U; 
    //wait for switching
    while(PLL1CONbits.OSWEN == 1U){}; 
    //wait for clock to be ready
    while(OSCCTRLbits.PLL1RDY == 0U){};    
    
    //Configure VCO Divider
    // INTDIV 4; 
    VCO1DIV = 0x40000UL;
    //enable PLL VCO divider
    PLL1CONbits.DIVSWEN = 1U; 
    //wait for setup complete
    while(PLL1CONbits.DIVSWEN == 1U){}; 
    
    // NOSC FRC Oscillator; OE enabled; SIDL disabled; ON enabled; BOSC FRC Oscillator; FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL External clock fail detection module #1; EXTCFEN disabled; RIS disabled; 
    CLK1CON = 0x000129500UL;
    // FRACDIV 0; INTDIV 0; 
    CLK1DIV = 0x0UL;
    //enable clock switching
    CLK1CONbits.OSWEN = 1U; 
    //wait for clock switching complete
    while(CLK1CONbits.OSWEN == 1U){};
    
    // NOSC FRC Oscillator; OE enabled; SIDL disabled; ON enabled; BOSC Serial Test Mode clock (PGC); FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL External clock fail detection module #1; EXTCFEN disabled; RIS disabled; 
    CLK2CON = 0x000129101UL;
    //enable clock switching
    CLK2CONbits.OSWEN = 1U; 
    //wait for clock switching complete
    while(CLK2CONbits.OSWEN == 1U){};
    
    // NOSC Backup FRC Oscillator; OE enabled; SIDL disabled; ON enabled; BOSC Serial Test Mode clock (PGC); FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL External clock fail detection module #1; EXTCFEN disabled; RIS disabled; 
    CLK3CON = 0x000129202UL;
    //enable clock switching
    CLK3CONbits.OSWEN = 1U; 
    //wait for clock switching complete
    while(CLK3CONbits.OSWEN == 1U){};    
    
    // Allow clocks to disable unless required by a peripheral
    CLK1CONbits.ON = 0;
    CLK2CONbits.ON = 0;
    CLK3CONbits.ON = 0;
    PLL1CONbits.ON = 0;
}





