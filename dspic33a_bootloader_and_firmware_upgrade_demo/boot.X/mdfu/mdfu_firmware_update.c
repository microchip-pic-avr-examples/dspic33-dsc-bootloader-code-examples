/*
Copyright (c) [2012-2024] Microchip Technology Inc.  

    All rights reserved.

    You are permitted to use the accompanying software and its derivatives 
    with Microchip products. See the Microchip license agreement accompanying 
    this software, if any, for additional info regarding your rights and 
    obligations.
    
    MICROCHIP SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT 
    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
    LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT 
    AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP OR ITS
    LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT 
    LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE 
    THEORY FOR ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT 
    LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
    OR OTHER SIMILAR COSTS. 
    
    To the fullest extend allowed by law, Microchip and its licensors 
    liability will not exceed the amount of fees, if any, that you paid 
    directly to Microchip to use this software. 
    
    THIRD PARTY SOFTWARE:  Notwithstanding anything to the contrary, any 
    third party software accompanying this software is subject to the terms 
    and conditions of the third party's license agreement.  To the extent 
    required by third party licenses covering such third party software, 
    the terms of such license will apply in lieu of the terms provided in 
    this notice or applicable license.  To the extent the terms of such 
    third party licenses prohibit any of the restrictions described here, 
    such restrictions will not apply to such third party software.
*/

#include <xc.h>

#include <stdlib.h>
#include "mdfu_firmware_update.h"
#include "mdfu_memory.h"
#include "mdfu_verification.h"
#include "mdfu_config.h"
#include "../mcc_generated_files/flash/flash_types.h"
#include "mdfu_partition.h"

extern struct PARTITION application;

/******************************************************************************/
/* Public Functions                                                           */
/******************************************************************************/
enum MDFU_FIRMWARE_UPDATE_RESULT MDFU_FirmwareUpdateWriteMemory(uint8_t* buffer, uint32_t length)
{
   static uint32_t currentAddress = MDFU_CONFIG_APPLICATION_ADDRESS_LOW;
   
   enum MDFU_FIRMWARE_UPDATE_RESULT status = MDFU_FIRMWARE_UPDATE_PROTOCOL_WRITE_ERROR;

   if (buffer == NULL)
   {
       status = MDFU_FIRMWARE_UPDATE_PROTOCOL_INVALID_BUFFER;
   } 
   else if  ((length == 0u) || (length > MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH))
   {
       status = MDFU_FIRMWARE_UPDATE_PROTOCOL_INVALID_LENGTH;
   }
   else 
   {
        if (MDFU_MemoryWriteChunk(currentAddress, length, buffer, FLASH_UNLOCK_KEY) == MDFU_MEMORY_PASS)
        {
            currentAddress += length;
            status = MDFU_FIRMWARE_UPDATE_PROTOCOL_PASS;
        }
        else
        {
            status = MDFU_FIRMWARE_UPDATE_PROTOCOL_WRITE_ERROR;
        }
   }
    return status;
}

enum MDFU_FIRMWARE_UPDATE_RESULT MDFU_FirmwareUpdateEraseMemory(void)
{
    enum MDFU_FIRMWARE_UPDATE_RESULT status = MDFU_FIRMWARE_UPDATE_PROTOCOL_FAIL;
    if (MDFU_MemoryEraseApplication(FLASH_UNLOCK_KEY) == MDFU_MEMORY_PASS)
    {
        status = MDFU_FIRMWARE_UPDATE_PROTOCOL_PASS;
    } 
    
    return status;
}

bool MDFU_FirmwareUpdateVerifyImage(void)
{
    return MDFU_Verify(&application) == MDFU_VERIFY_CODE_SUCCESS;
}

void MDFU_FirmwareUpdateStartApplication(void)
{
    int (*user_application)(void);
    
    /* This is probably being replaced at some point by __builtin__setIVTBASE() but is not currently supported */
    /* This also assumes that the first block of memory in the application is the reset/ivt table which may not always be true if there is an application header */
    PACCON1bits.IVTBASEWR = 1;
    IVTBASE = MDFU_CONFIG_APPLICATION_RESET_ADDRESS;
    PACCON1bits.IVTBASEWR = 0;
    
    uint32_t *resetVectorPtr = (uint32_t *)MDFU_CONFIG_APPLICATION_RESET_ADDRESS;
    uint32_t resetVector = *resetVectorPtr;

    user_application = (int(*)(void))resetVector;
    user_application();
}