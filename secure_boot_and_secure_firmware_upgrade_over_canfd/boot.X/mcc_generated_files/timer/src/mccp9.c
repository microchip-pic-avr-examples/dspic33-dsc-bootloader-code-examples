/**
 * MCCP9-TIMER Generated Driver Source File
 * 
 * @file      mccp9.c
 * 
 * @ingroup   timerdriver
 * 
 * @brief     This is the generated driver source file for MCCP9-TIMER driver
 *
 * @skipline @version   Firmware Driver Version 1.5.1
 *
 * @skipline @version   PLIB Version 1.6.2
 *
 * @skipline  Device : dsPIC33CK1024MP710
*/

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

// Section: Included Files

#include <stddef.h> 
#include "../mccp9.h"
#include "../timer_interface.h"

// Section: Data Type Definitions

#define MASK_32_BIT_LOW 0x0000FFFFU
#define MASK_32_BIT_HIGH 0xFFFF0000U

// Section: File specific functions

static void (*MCCP9_TimeoutHandler)(void) = NULL;

// Section: Driver Interface

// Defines an object for TIMER_INTERFACE

const struct TIMER_INTERFACE CAN_TP_TIMER = {
    .Initialize     = &MCCP9_Timer_Initialize,
    .Deinitialize   = &MCCP9_Timer_Deinitialize,
    .Start          = &MCCP9_Timer_Start,
    .Stop           = &MCCP9_Timer_Stop,
    #if TIMER_PERIODCOUNTSET_API_SUPPORT
    .PeriodCountSet = &MCCP9_Timer_PeriodCountSet,
    #endif
    .PeriodSet      = &MCCP9_Timer_PeriodSet,
    .CounterGet     = &MCCP9_Timer_CounterGet,
    .PeriodGet	    = &MCCP9_Timer_PeriodGet,
    .InterruptPrioritySet = &MCCP9_Timer_InterruptPrioritySet,
    .TimeoutCallbackRegister = &MCCP9_Timer_TimeoutCallbackRegister,
    .Tasks          = NULL,
};

// Section: Driver Interface Function Definitions

void MCCP9_Timer_Initialize(void)
{
    // MOD ; CCSEL disabled; TMR32 16 Bit; TMRPS 1:4; CLKSEL FOSC; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; 
    CCP9CON1L = 0x240; //The module is disabled, till other settings are configured
    //SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; 
    CCP9CON1H = 0x0;
    //ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; 
    CCP9CON2L = 0x0;
    //ICSEL ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN disabled; OCBEN disabled; OCCEN disabled; OCDEN disabled; OCEEN disabled; OCFEN disabled; OENSYNC disabled; 
    CCP9CON2H = 0x0;
    //PSSACE Tri-state; POLACE disabled; OUTM ; OSCNT None; OETRIG disabled; 
    CCP9CON3H = 0x0;
    //ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; 
    CCP9STATL = 0x0;
    //TMRL 0x0000; 
    CCP9TMRL = 0x0;
    //TMRH 0x0000; 
    CCP9TMRH = 0x0;
    //PRL 49999; 
    CCP9PRL = 0xC34F;
    //PRH 0; 
    CCP9PRH = 0x0;
    //CMPA 0; 
    CCP9RA = 0x0;
    //CMPB 0; 
    CCP9RB = 0x0;
    //BUFL 0x0000; 
    CCP9BUFL = 0x0;
    //BUFH 0x0000; 
    CCP9BUFH = 0x0;
    //DT 0; 
    CCP9CON3L = 0x0;
    
    MCCP9_Timer_TimeoutCallbackRegister(&MCCP9_TimeoutCallback);

    IFS11bits.CCT9IF = 0;
    // Enabling MCCP9 interrupt
    IEC11bits.CCT9IE = 1;

    CCP9CON1Lbits.CCPON = 1; //Enable Module
}

void MCCP9_Timer_Deinitialize(void)
{
    CCP9CON1Lbits.CCPON = 0;
    
    IFS11bits.CCT9IF = 0;
    IEC11bits.CCT9IE = 0;
    
    CCP9CON1L = 0x0; 
    CCP9CON1H = 0x0; 
    CCP9CON2L = 0x0; 
    CCP9CON2H = 0x100; 
    CCP9CON3H = 0x0; 
    CCP9STATL = 0x0; 
    CCP9TMRL = 0x0; 
    CCP9TMRH = 0x0; 
    CCP9PRL = 0xFFFF; 
    CCP9PRH = 0xFFFF; 
    CCP9RA = 0x0; 
    CCP9RB = 0x0; 
    CCP9BUFL = 0x0; 
    CCP9BUFH = 0x0; 
    CCP9CON3L = 0x0; 
}

void MCCP9_Timer_Start(void)
{
    IFS11bits.CCT9IF = 0;
    // Enable MCCP9 interrupt
    IEC11bits.CCT9IE = 1;
    
    CCP9CON1Lbits.CCPON = 1;
}

void MCCP9_Timer_Stop(void)
{
    CCP9CON1Lbits.CCPON = 0;
    
    IFS11bits.CCT9IF = 0;
    // Disable MCCP9 interrupt
    IEC11bits.CCT9IE = 0;
}

void MCCP9_Timer_PeriodSet(uint32_t count)
{
    if(count > 0xFFFFU)
    {
        CCP9PRL = (uint16_t)(count & MASK_32_BIT_LOW);
        CCP9PRH = (uint16_t)((count & MASK_32_BIT_HIGH) >> 16);
        CCP9CON1Lbits.T32 = 1;
    }
    else
    {
        CCP9PRL = (uint16_t)(count & MASK_32_BIT_LOW);
        CCP9CON1Lbits.T32 = 0;
    }
}

void MCCP9_Timer_InterruptPrioritySet(enum INTERRUPT_PRIORITY priority)
{
    IPC45bits.CCT9IP = priority;
}

void MCCP9_Timer_TimeoutCallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        MCCP9_TimeoutHandler = handler;
    }
}

void MCCP9_TimeoutCallbackRegister(void* handler)
{
    if(NULL != handler)
    {
        MCCP9_TimeoutHandler = handler;
    }
}

void __attribute__ ((weak)) MCCP9_TimeoutCallback (void)
{ 

} 

void __attribute__ ( ( interrupt, no_auto_psv ) ) _CCT9Interrupt (void)
{
    if(NULL != MCCP9_TimeoutHandler)
    {
        (*MCCP9_TimeoutHandler)();
    }
    IFS11bits.CCT9IF = 0;
}

void MCCP9_Timer_PeriodCountSet(size_t count)
{
    CCP9PRL = (uint16_t)(count & MASK_32_BIT_LOW);
    CCP9CON1Lbits.T32 = 0;
}

/**
 End of File
*/
