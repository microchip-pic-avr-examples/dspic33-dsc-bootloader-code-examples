/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_cam05346_wrapper.c

  Summary:
    Crypto Framework Library wrapper file for common CAM hardware management.

  Description:
    This source file contains the wrapper interface to manage common CAM hardware
    interactions for Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stddef.h>
#include <xc.h>
#include "crypto/drivers/wrapper/crypto_cam05346_wrapper.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Functions
// *****************************************************************************
// *****************************************************************************

#define CRYPTO_INT_HANDLER_COUNT (CRYPTO3_INT + 1)
struct crypto_Int_Handlers {
    crypto_Int_Handler handlers[CRYPTO_INT_HANDLER_COUNT];
};
static struct crypto_Int_Handlers cryptoIntHandlers = {{NULL}};

void __attribute__((interrupt)) _CRYPTO1Interrupt(void);
void __attribute__((interrupt)) _CRYPTO2Interrupt(void);
void __attribute__((interrupt)) _CRYPTO3Interrupt(void);


// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

void __attribute__((interrupt)) _CRYPTO1Interrupt(void)
{
    if (cryptoIntHandlers.handlers[CRYPTO1_INT] != NULL)
    {
        cryptoIntHandlers.handlers[CRYPTO1_INT]();
    }

    _CRYPT1IF = 0;
}

void __attribute__((interrupt)) _CRYPTO2Interrupt(void)
{
    if (cryptoIntHandlers.handlers[CRYPTO2_INT] != NULL)
    {
        cryptoIntHandlers.handlers[CRYPTO2_INT]();
    }

    _CRYPT2IF = 0;
}

void __attribute__((interrupt)) _CRYPTO3Interrupt(void)
{
    if (cryptoIntHandlers.handlers[CRYPTO3_INT] != NULL)
    {
        cryptoIntHandlers.handlers[CRYPTO3_INT]();
    }

    _CRYPT3IF = 0;
}

// *****************************************************************************
// *****************************************************************************
// Section: Interrupts Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

crypto_Int_Status_E Crypto_Int_Hw_Register_Handler(crypto_Int_Handler_Id handlerID, crypto_Int_Handler handler)
{
    crypto_Int_Status_E status = CRYPTO_INT_SUCCESS;

    if ((handlerID < CRYPTO1_INT) || (handlerID > CRYPTO3_INT))
    {
        status = CRYPTO_INT_INVALID_ID;
    }

    if (status == CRYPTO_INT_SUCCESS)
    {
        if ((handler != NULL) && (cryptoIntHandlers.handlers[handlerID] != NULL))
        {
            status = CRYPTO_INT_ALREADY_REGISTERED;
        }
    }

    if (status == CRYPTO_INT_SUCCESS)
    {
        cryptoIntHandlers.handlers[handlerID] = handler;
    }

    return status;
}

crypto_Int_Status_E Crypto_Int_Hw_Enable(crypto_Int_Handler_Id handlerID)
{
    crypto_Int_Status_E status = CRYPTO_INT_SUCCESS;

    switch (handlerID)
    {
        case CRYPTO1_INT:
            _CRYPT1IF = 0;
            _CRYPT1IE = 1;
            break;

        case CRYPTO2_INT:
            _CRYPT2IF = 0;
            _CRYPT2IE = 1;
            break;

        case CRYPTO3_INT:
            _CRYPT3IF = 0;
            _CRYPT3IE = 1;
            break;

        default:
            status = CRYPTO_INT_INVALID_ID;
            break;
    }

    return status;
}

crypto_Int_Status_E Crypto_Int_Hw_Disable(crypto_Int_Handler_Id handlerID)
{
    crypto_Int_Status_E status = CRYPTO_INT_SUCCESS;

    switch (handlerID)
    {
        case CRYPTO1_INT:
            _CRYPT1IF = 0;
            _CRYPT1IE = 0;
            break;

        case CRYPTO2_INT:
            _CRYPT2IF = 0;
            _CRYPT2IE = 0;
            break;

        case CRYPTO3_INT:
            _CRYPT3IF = 0;
            _CRYPT3IE = 0;
            break;

        default:
            status = CRYPTO_INT_INVALID_ID;
            break;
    }

    return status;
}
