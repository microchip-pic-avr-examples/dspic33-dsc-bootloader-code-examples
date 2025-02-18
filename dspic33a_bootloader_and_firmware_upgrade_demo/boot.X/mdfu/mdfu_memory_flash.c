/*
Copyright (c) [2012-2025] Microchip Technology Inc.  

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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "mdfu_memory.h"
#include "mdfu_config.h"
#include "../mcc_generated_files/flash/flash.h"
#include "../mcc_generated_files/flash/flash_types.h"

#define FLASH_ERASE_PAGE_SIZE_IN_BYTES (FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS * 4)

// Takes an exclusive end address and converts it to an inclusive end address of the last 
// instruction that is to be programmed. Each PC address increments by 4, thus going back one
// instruction is -4 to the address.
#define ConvertExclusiveToInclusiveAddress(address) ((address) - 4u)

#define MINIMUM_DEVICE_WRITE_SIZE_INSTRUCTIONS (4UL)
#define MINIMUM_DEVICE_WRITE_SIZE_BYTES (MINIMUM_DEVICE_WRITE_SIZE_INSTRUCTIONS * 4UL)

/******************************************************************************/
/* Private Function Prototypes                                                */
/******************************************************************************/
static bool IsLegalAddress(uint32_t addressToCheck);
static bool IsLegalRange(uint32_t startRangeToCheck, uint32_t endRangeToCheck);

/******************************************************************************/
/* Public Functions                                                           */
/******************************************************************************/
enum MDFU_MEMORY_RESULT MDFU_MemoryWriteChunk(uint32_t address, size_t length, uint8_t const * const data, uint32_t key)
{
    enum MDFU_MEMORY_RESULT status = MDFU_MEMORY_PASS;

    if(NULL == data)
    {
        status = MDFU_MEMORY_INVALID_ADDRESS;
    }
    else if (IsLegalRange(address, address + (length)) == false)
    {
        status = MDFU_MEMORY_INVALID_ADDRESS;
    }
    else
    {
        uint32_t count = 0;
        
        while(count < length)
        {
            uint32_t remaining = length - count;
            uint32_t flashData[MINIMUM_DEVICE_WRITE_SIZE_INSTRUCTIONS];
            uint32_t toCopy = remaining > MINIMUM_DEVICE_WRITE_SIZE_BYTES ? MINIMUM_DEVICE_WRITE_SIZE_BYTES : remaining;
            
            (void)memset(flashData, 0xFF, sizeof(flashData));
            (void)memcpy((void*)flashData, &data[count], toCopy);
                       
            if (FLASH_WordWrite(address+count, flashData, key) != FLASH_NO_ERROR)
            {
                status = MDFU_MEMORY_WRITE_ERROR;
                break;
            }
            
            count += toCopy;
        }
    }

    return status;
}

enum MDFU_MEMORY_RESULT MDFU_MemoryEraseApplication(uint32_t key) 
{
    enum MDFU_MEMORY_RESULT status = MDFU_MEMORY_PASS;

    for(uint32_t eraseAddress = MDFU_CONFIG_APPLICATION_ADDRESS_LOW; eraseAddress < MDFU_CONFIG_APPLICATION_ADDRESS_HIGH; eraseAddress += FLASH_ERASE_PAGE_SIZE_IN_BYTES)
    {
        uint32_t physicalEraseAddress = FLASH_ErasePageAddressGet(eraseAddress);
        if (FLASH_PageErase(physicalEraseAddress,  key) != FLASH_NO_ERROR)
        {
            status = MDFU_MEMORY_FAIL;
            break;
        }
    }
    return status;
}

/******************************************************************************/
/* Private Functions                                                          */
/******************************************************************************/
static bool IsLegalRange(uint32_t startRangeToCheck, uint32_t endRangeToCheck)
{
    return ( IsLegalAddress(startRangeToCheck) && IsLegalAddress(ConvertExclusiveToInclusiveAddress(endRangeToCheck)) );
}

static bool IsLegalAddress(uint32_t addressToCheck)
{
    return ((addressToCheck >= MDFU_CONFIG_APPLICATION_ADDRESS_LOW) && (addressToCheck <= MDFU_CONFIG_APPLICATION_ADDRESS_HIGH));
}