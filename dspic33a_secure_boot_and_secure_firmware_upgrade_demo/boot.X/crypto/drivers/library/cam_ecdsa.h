/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    cam_ecdsa.h

  Summary:
    Hardware abstraction layer header.

  Description:
    This header file defined the functions and structures for the CAM ECDSA
    hardware driver for the following families of Microchip microcontrollers:
    dsPIC33AK with Crypto Accelerator Module.
**************************************************************************/

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

#ifndef CAM_ECDSA_H
#define	CAM_ECDSA_H

#ifdef	__cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include "cam_pke.h"
#include "cam_version.h"

// *****************************************************************************
// *****************************************************************************
// Section: ECDSA Common Interface
// *****************************************************************************
// *****************************************************************************

/**
 *  @brief Initialize the ECDSA parameters used for signature generation.
 *  @param eccData Configuration structure to store ECDSA information.
 *  @param inputHash Hash input to use for the signature generation.
 *  @param hashLength Length of the Hash input.
 *  @param privateKey Private Key input for signature generation.
 *  @param privateKeyLength Length of the private key.
 *  @param eccCurve ECC Curve used for generation.
 *  @return CRYPTO_PKE_RESULT_SUCCESS on success.  CRYPTO_PKE_RESULT_ERROR_CURVE on failure.
 *  @note This function initializes eccData with ECDSA parameters and sets up the Hardware Crypto Engine.
 **/
CRYPTO_PKE_RESULT DRV_CRYPTO_ECDSA_InitEccParamsSign(PKE_CONFIG *eccData, uint8_t *inputHash, uint32_t hashLength, uint8_t * privateKey, uint32_t privateKeyLength, PKE_ECC_CURVE eccCurve);

/**
 *  @brief Generates a signature and sets the output to an input signature buffer.
 *  @param eccData Configuration structure to store ECDSA information.
 *  @param outputSignature Buffer to contain the signature.
 *  @param signatureLength Length of the signature
 *  @return CRYPTO_PKE_RESULT_SUCCESS on success.  CRYPTO_PKE_RESULT_ERROR_FAIL on failure.
 *  @note DRV_CRYPTO_ECDSA_InitEccParamsSign must be called before this function can be called.
 **/
CRYPTO_PKE_RESULT DRV_CRYPTO_ECDSA_Sign(PKE_CONFIG *eccData, uint8_t * outputSignature, uint32_t signatureLength);

/**
 *  @brief Initialize the ECDSA parameters for signature verification.
 *  @param eccData Configuration structure to store ECDSA information.
 *  @param inputHash Hash input to use for the signature verification.
 *  @param hashLength Length of the Hash input.
 *  @param inputSignature Signature input to verify.
 *  @param signatureLength Length of the signature.
 *  @param publicKey Public Key input for signature verification.
 *  @param publicKeyLength Length of the public key.
 *  @param eccCurve ECC Curve used for verification.
 *  @return CRYPTO_PKE_RESULT_SUCCESS on success.  CRYPTO_PKE_RESULT_ERROR_FAIL on failure.
 *  @note This function initializes eccData with ECDSA parameters and sets up the Hardware Crypto Engine.
 **/
CRYPTO_PKE_RESULT DRV_CRYPTO_ECDSA_InitEccParamsVerify(PKE_CONFIG *eccData, uint8_t *inputHash, 
        uint32_t hashLength, uint8_t *inputSignature, uint32_t signatureLength, uint8_t *publicKey,
        uint32_t publicKeyLength, PKE_ECC_CURVE eccCurve);

/**
 *  @brief Executes the Signature verification and returns the result.
 *  @param eccData Configuration structure to store ECDSA information.
 *  @return CRYPTO_PKE_RESULT_SUCCESS on success.  CRYPTO_PKE_RESULT_ERROR_FAIL on failure.
 *  @note DRV_CRYPTO_ECDSA_InitEccParamsVerify must be called before this function can be called.
 **/
CRYPTO_PKE_RESULT DRV_CRYPTO_ECDSA_Verify(PKE_CONFIG *eccData);

#ifdef	__cplusplus
}
#endif

#endif	/* CAM_ECDSA_H */