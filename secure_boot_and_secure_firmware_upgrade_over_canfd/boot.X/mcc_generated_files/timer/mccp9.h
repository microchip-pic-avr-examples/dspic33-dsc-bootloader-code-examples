/**
 * MCCP9-TIMER Generated Driver Header File 
 * 
 * @file      mccp9.h
 * 
 * @ingroup   timerdriver
 * 
 * @brief     This is the generated driver header file for the MCCP9-TIMER driver
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

#ifndef MCCP9_H
#define MCCP9_H

// Section: Included Files

#include <stddef.h>
#include <stdint.h>
#include <xc.h>
#include "timer_interface.h"

// Section: Data Type Definitions


/**
 * @ingroup  timerdriver
 * @brief    Structure object of type TIMER_INTERFACE with the custom name given by 
 *           the user in the Melody Driver User interface. The default name 
 *           e.g. Timer1 can be changed by the user in the TIMER user interface. 
 *           This allows defining a structure with application specific name using 
 *           the 'Custom Name' field. Application specific name allows the API Portability.
*/
extern const struct TIMER_INTERFACE CAN_TP_TIMER;

/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_Initialize API
 */
#define CAN_TP_TIMER_Initialize MCCP9_Timer_Initialize
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_Deinitialize API
 */
#define CAN_TP_TIMER_Deinitialize MCCP9_Timer_Deinitialize
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_Tasks API
 */
#define CAN_TP_TIMER_Tasks MCCP9_Timer_Tasks
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_Start API
 */
#define CAN_TP_TIMER_Start MCCP9_Timer_Start
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_Stop API
 */
#define CAN_TP_TIMER_Stop MCCP9_Timer_Stop

#if TIMER_PERIODCOUNTSET_API_SUPPORT
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_PeriodCountSet API
 */
#define CAN_TP_TIMER_PeriodCountSet MCCP9_Timer_PeriodCountSet
#endif

/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_PeriodSet API
 */
#define CAN_TP_TIMER_PeriodSet MCCP9_Timer_PeriodSet
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_PeriodGet API
 */
#define CAN_TP_TIMER_PeriodGet MCCP9_Timer_PeriodGet
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_CounterGet API
 */
#define CAN_TP_TIMER_CounterGet MCCP9_Timer_CounterGet
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_Counter16BitGet API
 */
#define CAN_TP_TIMER_Counter16BitGet MCCP9_Timer_Counter16BitGet
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_InterruptPrioritySet API
 */
#define CAN_TP_TIMER_InterruptPrioritySet MCCP9_Timer_InterruptPrioritySet

/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref MCCP9_Timer_TimeoutCallbackRegister API
 */
#define CAN_TP_TIMER_TimeoutCallbackRegister MCCP9_Timer_TimeoutCallbackRegister

// Section: Driver Interface Functions

/**
 * @ingroup  timerdriver
 * @brief    Initializes the MCCP9 module 
 * @param    none
 * @return   none  
 */
void MCCP9_Timer_Initialize (void);

/**
 * @ingroup  timerdriver
 * @brief    Deinitializes the MCCP9 to POR values
 * @param    none
 * @return   none  
 */
void MCCP9_Timer_Deinitialize(void);

/**
 * @ingroup  timerdriver
 * @brief    Starts the timer
 * @pre      \ref MCCP9_Timer_Initialize must be called
 * @param    none
 * @return   none  
 */
void MCCP9_Timer_Start(void);

/**
 * @ingroup  timerdriver
 * @brief    Stops the timer
 * @pre      \ref MCCP9_Timer_Initialize must be called
 * @param    none
 * @return   none  
 */
void MCCP9_Timer_Stop(void);

/**
 * @ingroup  timerdriver
 * @brief    Sets the MCCP9-Timer period count value
 * @pre      \ref MCCP9_Timer_Initialize must be called
 * @param[in]  count - period value
 * @return   none  
 */
void MCCP9_Timer_PeriodSet(uint32_t count);

/**
 * @ingroup  timerdriver
 * @brief    This inline function gets the MCCP9-Timer period count value
 * @pre      \ref MCCP9_Timer_Initialize must be called
 * @param    none
 * @return   Period count value  
 */
inline static uint32_t MCCP9_Timer_PeriodGet(void)
{
    if(CCP9CON1Lbits.T32 == 1)
    {
        return (((uint32_t)CCP9PRH << 16U) | (CCP9PRL) );
    }
    else
    {
        return (uint32_t) CCP9PRL;
    }
}

/**
 * @ingroup  timerdriver
 * @brief    This inline function gets the MCCP9-Timer elapsed count value
 * @param    none
 * @return   Elapsed count value of the timer  
 */
inline static uint32_t MCCP9_Timer_CounterGet(void)
{
    if(CCP9CON1Lbits.T32 == 1)
    {
        return (((uint32_t)CCP9TMRH << 16U) | CCP9TMRL);
    }
    else
    {
        return (uint32_t)CCP9TMRL;
    }
}

/**
 * @ingroup  timerdriver
 * @brief    This inline function gets the MCCP9-Timer least significant 16 bit elapsed count value
 * @param    none
 * @return   Least significant 16 bit elapsed count value of the timer  
 */
inline static uint16_t MCCP9_Timer_Counter16BitGet(void)
{
    return CCP9TMRL;
}

/**
 * @ingroup  timerdriver
 * @brief    Sets the Interrupt Priority Value 
 * @param    none
 * @return   none  
 */
void MCCP9_Timer_InterruptPrioritySet(enum INTERRUPT_PRIORITY priority);


/**
 * @ingroup    timerdriver
 * @brief      This function can be used to override default callback and to define 
 *             custom callback for MCCP9 Timeout event.
 * @param[in]  handler - Address of the callback function.  
 * @return     none 
 */
void MCCP9_Timer_TimeoutCallbackRegister(void (*handler)(void));

/**
 * @ingroup    timerdriver
 * @brief      This function can be used to override default callback and to define 
 *             custom callback for MCCP9 Timeout event.
 * @param[in]  handler - Address of the callback function.  
 * @return     none 
 */
void MCCP9_TimeoutCallbackRegister(void* handler)__attribute__((deprecated("\nThis will be removed in future MCC releases. \nUse MCCP9_Timer_TimeoutCallbackRegister instead. ")));

/**
 * @ingroup  timerdriver
 * @brief    This is the default callback with weak attribute. The user can 
 *           override and implement the default callback without weak attribute 
 *           or can register a custom callback function using  \ref MCCP9_Timer_TimeoutCallbackRegister.
 * @param    none
 * @return   none  
 */
void MCCP9_TimeoutCallback(void);


#if TIMER_PERIODCOUNTSET_API_SUPPORT
/**
 * @ingroup  timerdriver
 * @brief    Sets the MCCP9-Timer period count value
 * @pre      \ref MCCP9_Timer_Initialize must be called
 * @param[in]  count - period value
 * @return   none  
 */
void MCCP9_Timer_PeriodCountSet(size_t count)__attribute__((deprecated ("\nThis will be removed in future MCC releases. \nUse MCCP9_Timer_PeriodSet instead. ")));
#endif
#endif //MCCP9_H

/**
 End of File
*/


