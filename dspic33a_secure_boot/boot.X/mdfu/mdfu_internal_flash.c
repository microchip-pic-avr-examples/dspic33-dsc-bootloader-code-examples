/*
Copyright (c) [2025] Microchip Technology Inc.  

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

#include "mdfu_partition.h"
#include "mdfu_config.h"
#include "mdfu_internal_flash.h"
#include "../mcc_generated_files/flash/flash.h"
#include "../mcc_generated_files/flash/flash_types.h"

#include <string.h>
#include <stdint.h>

#include <xc.h>

// Takes an exclusive end address and converts it to an inclusive end address of the last 
// instruction that is to be programmed. Each PC address increments by 4, thus going back one
// instruction is -4 to the address.
#define ConvertExclusiveToInclusiveAddress(address) ((address) - 4u)

#define MINIMUM_DEVICE_WRITE_SIZE_INSTRUCTIONS (4UL)
#define MINIMUM_DEVICE_WRITE_SIZE_BYTES (MINIMUM_DEVICE_WRITE_SIZE_INSTRUCTIONS * 4UL)

#define FLASH_ERASE_PAGE_SIZE_IN_BYTES (FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS * 4UL)

/**
 * @brief      Erases a requested block of memory.
 * @param[in]  startAddress of where to start erasing, must be erase page aligned
 * @param[in]  length of how much data to erase, must be erase page aligned
 * @return     bool - true if the erase operation was completely successful.  false otherwise.
 */
bool MDFU_INTERNAL_FLASH_BlockErase(uint32_t startAddress, uint32_t length)
{
    bool status = true;
    
    if((startAddress % FLASH_ERASE_PAGE_SIZE_IN_BYTES) != 0UL){
        status = false;
    } else if((length % FLASH_ERASE_PAGE_SIZE_IN_BYTES) != 0UL){
        status = false;
    } else {
        uint32_t endAddress = startAddress + length;
        
        for(uint32_t eraseAddress = startAddress; eraseAddress < endAddress; eraseAddress += FLASH_ERASE_PAGE_SIZE_IN_BYTES)
        {
            uint32_t physicalEraseAddress = FLASH_ErasePageAddressGet(eraseAddress);
            if (FLASH_PageErase(physicalEraseAddress, FLASH_UNLOCK_KEY) != FLASH_NO_ERROR)
            {
                status = false;
                break;
            }
        }
    }
    return status;
}

/**
 * @brief    Writes data to the specified address in memory. 
 * @param    size_t length - Number of bytes to be written
 * @param    uint8_t const * const - Pointer to the buffer containing data to be written
 * @return   bool - true if write operation was completely successful.  false if any portion of write failed
 */
bool MDFU_INTERNAL_FLASH_Write(uint32_t address, size_t length, uint8_t const * const data)
{
    bool status = true;
    uint32_t count = 0;

    if(NULL == data){
        status = false;
    } else {
        while(count < length)
        {
            uint32_t remaining = length - count;
            uint32_t flashData[MINIMUM_DEVICE_WRITE_SIZE_INSTRUCTIONS];
            uint32_t toCopy = remaining > MINIMUM_DEVICE_WRITE_SIZE_BYTES ? MINIMUM_DEVICE_WRITE_SIZE_BYTES : remaining;

            (void)memset(flashData, 0xFF, sizeof(flashData));
            (void)memcpy((void*)flashData, &data[count], toCopy);

            if (FLASH_WordWrite(address+count, flashData, FLASH_UNLOCK_KEY) != FLASH_NO_ERROR)
            {
                status = false;
                break;
            }

            count += toCopy;
        }
    }

    return status;
}

