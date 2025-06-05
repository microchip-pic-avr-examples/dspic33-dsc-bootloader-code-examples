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
#include "mdfu_internal_flash.h"
#include "mdfu_verification.h"
#include "mdfu_config.h"
#include "../mcc_generated_files/flash/flash_types.h"
#include "mdfu_partition.h"

extern struct MDFU_PARTITION executable;
static uint32_t currentOffset = 0;

/******************************************************************************/
/* Public Functions                                                           */
/******************************************************************************/
void MDFU_FirmwareUpdateInitialize(void)
{
    currentOffset = 0;
}

enum MDFU_FIRMWARE_UPDATE_RESULT MDFU_FirmwareUpdateWriteMemory(const struct MDFU_PARTITION* partition, uint8_t* buffer, uint32_t length)
{   
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
        if (partition->write(currentOffset, length, buffer) == MDFU_PARTITION_STATUS_SUCCESS)
        {
            currentOffset += length;
            status = MDFU_FIRMWARE_UPDATE_PROTOCOL_PASS;
        }
        else
        {
            status = MDFU_FIRMWARE_UPDATE_PROTOCOL_WRITE_ERROR;
        }
   }
    return status;
}

enum MDFU_FIRMWARE_UPDATE_RESULT MDFU_FirmwareUpdateEraseMemory(const struct MDFU_PARTITION* partition)
{
    enum MDFU_FIRMWARE_UPDATE_RESULT status = MDFU_FIRMWARE_UPDATE_PROTOCOL_FAIL;
    if (partition->erase() == MDFU_PARTITION_STATUS_SUCCESS)
    {
        status = MDFU_FIRMWARE_UPDATE_PROTOCOL_PASS;
    } 
    
    return status;
}
