/**
 * PINS Generated Driver Header File 
 * 
 * @file      pins.h
 *            
 * @defgroup  pinsdriver Pins Driver
 *            
 * @brief     The Pin Driver directs the operation and function of 
 *            the selected device pins using dsPIC MCUs.
 *
 * @skipline @version   Firmware Driver Version 1.0.2
 *
 * @skipline @version   PLIB Version 1.3.0
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

#ifndef PINS_H
#define PINS_H
// Section: Includes
#include <xc.h>

// Section: Device Pin Macros

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA6 GPIO Pin which has a custom name of IO_RA6 to High
 * @pre      The RA6 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define IO_RA6_SetHigh()          (_LATA6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA6 GPIO Pin which has a custom name of IO_RA6 to Low
 * @pre      The RA6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define IO_RA6_SetLow()           (_LATA6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RA6 GPIO Pin which has a custom name of IO_RA6
 * @pre      The RA6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define IO_RA6_Toggle()           (_LATA6 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RA6 GPIO Pin which has a custom name of IO_RA6
 * @param    none
 * @return   none  
 */
#define IO_RA6_GetValue()         _RA6

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA6 GPIO Pin which has a custom name of IO_RA6 as Input
 * @param    none
 * @return   none  
 */
#define IO_RA6_SetDigitalInput()  (_TRISA6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA6 GPIO Pin which has a custom name of IO_RA6 as Output
 * @param    none
 * @return   none  
 */
#define IO_RA6_SetDigitalOutput() (_TRISA6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD15 GPIO Pin which has a custom name of IO_RD15 to High
 * @pre      The RD15 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define IO_RD15_SetHigh()          (_LATD15 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD15 GPIO Pin which has a custom name of IO_RD15 to Low
 * @pre      The RD15 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define IO_RD15_SetLow()           (_LATD15 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD15 GPIO Pin which has a custom name of IO_RD15
 * @pre      The RD15 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define IO_RD15_Toggle()           (_LATD15 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD15 GPIO Pin which has a custom name of IO_RD15
 * @param    none
 * @return   none  
 */
#define IO_RD15_GetValue()         _RD15

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD15 GPIO Pin which has a custom name of IO_RD15 as Input
 * @param    none
 * @return   none  
 */
#define IO_RD15_SetDigitalInput()  (_TRISD15 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD15 GPIO Pin which has a custom name of IO_RD15 as Output
 * @param    none
 * @return   none  
 */
#define IO_RD15_SetDigitalOutput() (_TRISD15 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE9 GPIO Pin which has a custom name of IO_RE9 to High
 * @pre      The RE9 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define IO_RE9_SetHigh()          (_LATE9 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE9 GPIO Pin which has a custom name of IO_RE9 to Low
 * @pre      The RE9 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define IO_RE9_SetLow()           (_LATE9 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RE9 GPIO Pin which has a custom name of IO_RE9
 * @pre      The RE9 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define IO_RE9_Toggle()           (_LATE9 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RE9 GPIO Pin which has a custom name of IO_RE9
 * @param    none
 * @return   none  
 */
#define IO_RE9_GetValue()         _RE9

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE9 GPIO Pin which has a custom name of IO_RE9 as Input
 * @param    none
 * @return   none  
 */
#define IO_RE9_SetDigitalInput()  (_TRISE9 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE9 GPIO Pin which has a custom name of IO_RE9 as Output
 * @param    none
 * @return   none  
 */
#define IO_RE9_SetDigitalOutput() (_TRISE9 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE14 GPIO Pin which has a custom name of IO_RE14 to High
 * @pre      The RE14 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define IO_RE14_SetHigh()          (_LATE14 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE14 GPIO Pin which has a custom name of IO_RE14 to Low
 * @pre      The RE14 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define IO_RE14_SetLow()           (_LATE14 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RE14 GPIO Pin which has a custom name of IO_RE14
 * @pre      The RE14 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define IO_RE14_Toggle()           (_LATE14 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RE14 GPIO Pin which has a custom name of IO_RE14
 * @param    none
 * @return   none  
 */
#define IO_RE14_GetValue()         _RE14

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE14 GPIO Pin which has a custom name of IO_RE14 as Input
 * @param    none
 * @return   none  
 */
#define IO_RE14_SetDigitalInput()  (_TRISE14 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE14 GPIO Pin which has a custom name of IO_RE14 as Output
 * @param    none
 * @return   none  
 */
#define IO_RE14_SetDigitalOutput() (_TRISE14 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Initializes the PINS module
 * @param    none
 * @return   none  
 */
void PINS_Initialize(void);



#endif
