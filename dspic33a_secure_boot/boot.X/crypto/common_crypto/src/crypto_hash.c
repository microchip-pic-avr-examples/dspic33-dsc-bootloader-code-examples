/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_hash.c

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

#include "crypto/common_crypto/crypto_common.h"
#include "crypto/common_crypto/crypto_hash.h"
#include "crypto/drivers/wrapper/crypto_hash_cam05346_wrapper.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define CRYPTO_HASH_SESSION_MAX (1)

// *****************************************************************************
// *****************************************************************************
// Section: Function Definitions
// *****************************************************************************
// *****************************************************************************

//SHA-1, SHA-2 and SHA-3 except SHAKE Algorithm
crypto_Hash_Status_E Crypto_Hash_Sha_Digest(crypto_HandlerType_E shaHandler_en, uint8_t *ptr_data, uint32_t dataLen,
                                                uint8_t *ptr_digest, crypto_Hash_Algo_E shaAlgorithm_en,  uint32_t shaSessionId)
{
 	crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;

    if( (ptr_data == NULL) && (dataLen != 0u) )
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else if(ptr_digest == NULL)
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
    else if( (shaAlgorithm_en <= CRYPTO_HASH_INVALID) || (shaAlgorithm_en >= CRYPTO_HASH_MAX))
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_ALGO;
    }
    else if( (shaSessionId == 0u) || (shaSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_SID;
    }
    else
    {
        switch(shaHandler_en)
        {
            case CRYPTO_HANDLER_HW_INTERNAL:
                ret_shaStat_en = Crypto_Hash_Hw_Sha_Digest((void*)ptr_data, dataLen, ptr_digest, shaAlgorithm_en);
                break;
            default:
                ret_shaStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
    return ret_shaStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Sha_Init(st_Crypto_Hash_Sha_Ctx *ptr_shaCtx_st, crypto_Hash_Algo_E shaAlgorithm_en, crypto_HandlerType_E shaHandler_en, uint32_t shaSessionId)
{
 	crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;

    if(ptr_shaCtx_st == NULL)
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if( (shaAlgorithm_en <= CRYPTO_HASH_INVALID) || (shaAlgorithm_en >= CRYPTO_HASH_MAX))
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_ALGO;
    }
    else if( (shaSessionId == 0u) || (shaSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_SID;
    }
    else
    {
        ptr_shaCtx_st->shaSessionId = shaSessionId;
        ptr_shaCtx_st->shaAlgo_en = shaAlgorithm_en;
        ptr_shaCtx_st->shaHandler_en = shaHandler_en;

        switch(ptr_shaCtx_st->shaHandler_en)
        {
            case CRYPTO_HANDLER_HW_INTERNAL:
                ret_shaStat_en = Crypto_Hash_Hw_Sha_Init((void*)ptr_shaCtx_st->arr_shaDataCtx, ptr_shaCtx_st->shaAlgo_en);
                break;
            default:
                ret_shaStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
    return ret_shaStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Sha_Update(st_Crypto_Hash_Sha_Ctx *ptr_shaCtx_st, uint8_t *ptr_data, uint32_t dataLen)
{
	crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;

    if(ptr_shaCtx_st == NULL)
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if( (ptr_data == NULL) && (dataLen != 0u) )
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else
    {
        switch(ptr_shaCtx_st->shaHandler_en)
        {
            case CRYPTO_HANDLER_HW_INTERNAL:
                ret_shaStat_en = Crypto_Hash_Hw_Sha_Update((void*)ptr_shaCtx_st->arr_shaDataCtx, ptr_data, dataLen);
                break;
            default:
                ret_shaStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
    return ret_shaStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Sha_Final(st_Crypto_Hash_Sha_Ctx *ptr_shaCtx_st, uint8_t *ptr_digest)
{
	crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;

    if(ptr_shaCtx_st == NULL)
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if(ptr_digest == NULL)
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
    else
    {
        switch(ptr_shaCtx_st->shaHandler_en)
        {
            case CRYPTO_HANDLER_HW_INTERNAL:
                ret_shaStat_en = Crypto_Hash_Hw_Sha_Final((void*)ptr_shaCtx_st->arr_shaDataCtx, ptr_digest);
                break;
            default:
                ret_shaStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
    return ret_shaStat_en;
}

