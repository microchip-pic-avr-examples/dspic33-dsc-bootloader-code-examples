/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_digsign.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2025 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "crypto/common_crypto/crypto_digsign.h"
#include "crypto/common_crypto/crypto_common.h"
#include "crypto/drivers/wrapper/crypto_digisign_cam05346_wrapper.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// ***************************************************************************** 

#define CRYPTO_DIGISIGN_SESSION_MAX (1) 

// *****************************************************************************
// *****************************************************************************
// Section: Function Definitions
// *****************************************************************************
// *****************************************************************************

crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_Sign(crypto_HandlerType_E ecdsaHandlerType_en, uint8_t *ptr_inputHash, uint32_t hashLen, uint8_t *ptr_outSig, 
                                                    uint32_t sigLen, uint8_t *ptr_privKey, uint32_t privKeyLen, crypto_EccCurveType_E eccCurveType_En, uint32_t ecdsaSessionId)
{
    crypto_DigiSign_Status_E ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_ALGONOTSUPPTD;
    
    if( (ptr_inputHash == NULL) || (hashLen == 0u) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_INPUTHASH;
    }
    else if( (ptr_outSig == NULL) || (sigLen == 0u) )
    {
         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_SIGNATURE;
    }
    else if( (eccCurveType_En <= CRYPTO_ECC_CURVE_INVALID) || (eccCurveType_En >= CRYPTO_ECC_CURVE_MAX) )
    {
         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_FAIL;
    }
    else if( (ptr_privKey == NULL) || (privKeyLen == 0u))
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_PRIVKEY;
    }
    else if((ecdsaSessionId == 0u) || (ecdsaSessionId > (uint32_t)CRYPTO_DIGISIGN_SESSION_MAX) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_SID;
    }
    else
    {
        switch(ecdsaHandlerType_en)
        {
            case CRYPTO_HANDLER_HW_INTERNAL:
            	ret_ecdsaStat_en = Crypto_DigiSign_Ecdsa_Hw_Sign(ptr_inputHash, hashLen, ptr_outSig, sigLen, ptr_privKey, privKeyLen, eccCurveType_En);            	
                break;             
            default:
                ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_HDLR;
                break;
        }
    }
    return ret_ecdsaStat_en;
}

crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_Verify(crypto_HandlerType_E ecdsaHandlerType_en, uint8_t *ptr_inputHash, uint32_t hashLen, uint8_t *ptr_inputSig, uint32_t sigLen, 
                                                    uint8_t *ptr_pubKey, uint32_t pubKeyLen, int8_t *ptr_sigVerifyStat, crypto_EccCurveType_E eccCurveType_En, uint32_t ecdsaSessionId)
{
    crypto_DigiSign_Status_E ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_ALGONOTSUPPTD;
    
    if( (ptr_inputHash == NULL) || (hashLen == 0u) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_INPUTHASH;
    }
    else if( (ptr_inputSig == NULL) || (sigLen == 0u) )
    {
         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_SIGNATURE;
    }
    else if( (eccCurveType_En <= CRYPTO_ECC_CURVE_INVALID) || (eccCurveType_En >= CRYPTO_ECC_CURVE_MAX) )
    {
         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_CURVE;
    }
    else if( (ptr_pubKey == NULL) || (pubKeyLen == 0u) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_PUBKEY;
    }
    //Check the Key compression Type, 0x04 for uncompressed, 0x02 for Even compressed and 0x03 for Odd compressed
    else if( !( (ptr_pubKey[0] == 0x04u) || ((ptr_pubKey[0] == 0x02u) || (ptr_pubKey[0] == 0x03u)) ) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_PUBKEYCOMPRESS;
    }
    else if((ecdsaSessionId == 0u) || (ecdsaSessionId > (uint32_t)CRYPTO_DIGISIGN_SESSION_MAX) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_SID;
    }
    else
    {
        switch(ecdsaHandlerType_en)
        {
            case CRYPTO_HANDLER_HW_INTERNAL:
            	ret_ecdsaStat_en = Crypto_DigiSign_Ecdsa_Hw_Verify(ptr_inputHash, hashLen, ptr_inputSig, sigLen, ptr_pubKey, pubKeyLen, 
                                        ptr_sigVerifyStat, eccCurveType_En);
            	break;
            default:
                ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_HDLR;
                break;
        }
    }
    return ret_ecdsaStat_en;
}




