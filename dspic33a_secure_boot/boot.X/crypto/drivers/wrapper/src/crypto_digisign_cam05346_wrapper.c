/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_digisign_cam05346_wrapper.c

  Summary:
    Crypto Framework Library wrapper file for the digital signature in the
    hardware cryptographic library.

  Description:
    This source file contains the wrapper interface to access the hardware
    cryptographic library in Microchip microcontrollers for digital signature.
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
#include <xc.h>
#include "crypto/drivers/wrapper/crypto_digisign_cam05346_wrapper.h"
#include "crypto/drivers/wrapper/crypto_cam05346_wrapper.h"
#include "crypto/drivers/library/cam_ecdsa.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Function Implementations
// *****************************************************************************
// *****************************************************************************

static void lDRV_CRYPTO_ECDSA_InterruptSetup(void)
{
    (void)Crypto_Int_Hw_Register_Handler(CRYPTO3_INT, DRV_CRYPTO_PKE_IsrHelper);
    (void)Crypto_Int_Hw_Enable(CRYPTO3_INT);
}

static CRYPTO_PKE_RESULT lCrypto_DigSign_Ecdsa_Hw_GetCurve(
    crypto_EccCurveType_E eccCurveType, PKE_ECC_CURVE *hwEccCurve)
{
    CRYPTO_PKE_RESULT eccStatus = CRYPTO_PKE_RESULT_SUCCESS;

    switch (eccCurveType)
    {
        case CRYPTO_ECC_CURVE_P192:
            *hwEccCurve = P192;
            break;
        
        case CRYPTO_ECC_CURVE_P256:
            *hwEccCurve = P256;
            break;
        
        case CRYPTO_ECC_CURVE_P384:
            *hwEccCurve = P384;
            break;
        
        case CRYPTO_ECC_CURVE_P521:
            *hwEccCurve = P521;
            break;
            
        default:
            eccStatus = CRYPTO_PKE_RESULT_ERROR_CURVE;
            break;
    }
    
    return eccStatus;
}

static crypto_DigiSign_Status_E lCrypto_DigSign_Ecdsa_Hw_MapResult(CRYPTO_PKE_RESULT result)
{
    crypto_DigiSign_Status_E digiSigntatus;

    switch (result)
    {
        case CRYPTO_PKE_RESULT_SUCCESS:
            digiSigntatus = CRYPTO_DIGISIGN_SUCCESS;
            break;

        case CRYPTO_PKE_ERROR_PUBKEYCOMPRESS:
            digiSigntatus = CRYPTO_DIGISIGN_ERROR_PUBKEYCOMPRESS;
            break;

        case CRYPTO_PKE_RESULT_ERROR_CURVE:
            digiSigntatus = CRYPTO_DIGISIGN_ERROR_CURVE;
            break;

        case CRYPTO_PKE_RESULT_ERROR_RNG:
            digiSigntatus = CRYPTO_DIGISIGN_ERROR_RNG;
            break;

        case CRYPTO_PKE_RESULT_INIT_FAIL:
        case CRYPTO_PKE_RESULT_ERROR_FAIL:
            digiSigntatus = CRYPTO_DIGISIGN_ERROR_FAIL;
            break;

        default:
            digiSigntatus = CRYPTO_DIGISIGN_ERROR_FAIL;
            break;
    }

    return digiSigntatus;
}

// *****************************************************************************
// *****************************************************************************
// Section: DigSign Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_Hw_Sign(uint8_t *inputHash,
    uint32_t hashLen, uint8_t *outSig, uint32_t sigLen, uint8_t *privKey,
    uint32_t privKeyLen, crypto_EccCurveType_E eccCurveType_En)
{
    CRYPTO_PKE_RESULT hwResult;
    PKE_ECC_CURVE hwEccCurve;
    PKE_CONFIG eccData;

    /* Get curve */
    hwResult = lCrypto_DigSign_Ecdsa_Hw_GetCurve(eccCurveType_En, &hwEccCurve);

    if (hwResult == CRYPTO_PKE_RESULT_SUCCESS)
    {
        /* Initialize the hardware library for ECDSA signature */
        hwResult = DRV_CRYPTO_ECDSA_InitEccParamsSign(&eccData, inputHash, hashLen, privKey, privKeyLen, hwEccCurve);
    }
    
    lDRV_CRYPTO_ECDSA_InterruptSetup();
    
    if(hwResult == CRYPTO_PKE_RESULT_SUCCESS)
    {
        /* Generate the signature */
        hwResult = DRV_CRYPTO_ECDSA_Sign(&eccData, outSig, sigLen);
    }
    
    return lCrypto_DigSign_Ecdsa_Hw_MapResult(hwResult);
}

crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_Hw_Verify(uint8_t *inputHash,
    uint32_t hashLen, uint8_t *inputSig, uint32_t sigLen, uint8_t *pubKey,
    uint32_t pubKeyLen, int8_t *hashVerifyStatus,
    crypto_EccCurveType_E eccCurveType_En)
{
    CRYPTO_PKE_RESULT hwResult;
    PKE_ECC_CURVE hwEccCurve;
    PKE_CONFIG eccData;

    /* Get curve */
    hwResult = lCrypto_DigSign_Ecdsa_Hw_GetCurve(eccCurveType_En, &hwEccCurve);
    
    if(hwResult == CRYPTO_PKE_RESULT_SUCCESS)
    {
        /* Initialize the hardware library for ECDSA signature verification */
        hwResult = DRV_CRYPTO_ECDSA_InitEccParamsVerify(&eccData,
                                                        inputHash,
                                                        hashLen,
                                                        inputSig,
                                                        sigLen,
                                                        pubKey,
                                                        pubKeyLen,
                                                        hwEccCurve);
    }
    
    lDRV_CRYPTO_ECDSA_InterruptSetup();
    
    if(hwResult == CRYPTO_PKE_RESULT_SUCCESS)
    {
        /* Verify the signature */
        hwResult = DRV_CRYPTO_ECDSA_Verify(&eccData);
    }
    
    /* Set verification status */
    if (hwResult == CRYPTO_PKE_RESULT_SUCCESS)
    {
        *hashVerifyStatus = 1;
    }
    else
    {
        *hashVerifyStatus = 0;
    }

    return lCrypto_DigSign_Ecdsa_Hw_MapResult(hwResult);
}