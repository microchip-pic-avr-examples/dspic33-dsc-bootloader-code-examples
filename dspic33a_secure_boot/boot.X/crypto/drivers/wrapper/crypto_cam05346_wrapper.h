/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_cam05346_wrapper.h

  Summary:
    Crypto Framework Library wrapper file for common CAM hardware management.

  Description:
    This header file contains the wrapper interface to manage common CAM hardware
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

#ifndef MCHP_CRYPTO_CAM05346_WRAPPER_H
#define MCHP_CRYPTO_CAM05346_WRAPPER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef enum crypto_Int_Status_E {
    CRYPTO_INT_SUCCESS = 0,
    CRYPTO_INT_INVALID_ID = -1,
    CRYPTO_INT_ALREADY_REGISTERED = -2,
    CRYPTO_INT_GENERAL_FAIL = -127

} crypto_Int_Status_E;

typedef enum crypto_Int_Handler_Id {
    CRYPTO1_INT = 0,
    CRYPTO2_INT = 1,
    CRYPTO3_INT = 2,
} crypto_Int_Handler_Id;

typedef void (*crypto_Int_Handler)(void);

// *****************************************************************************
// *****************************************************************************
// Section: Interrupts Common Interface
// *****************************************************************************
// *****************************************************************************

crypto_Int_Status_E Crypto_Int_Hw_Register_Handler(crypto_Int_Handler_Id handlerID, crypto_Int_Handler handler);
crypto_Int_Status_E Crypto_Int_Hw_Enable(crypto_Int_Handler_Id handlerID);
crypto_Int_Status_E Crypto_Int_Hw_Disable(crypto_Int_Handler_Id handlerID);


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif /* MCHP_CRYPTO_CAM05346_WRAPPER_H */
