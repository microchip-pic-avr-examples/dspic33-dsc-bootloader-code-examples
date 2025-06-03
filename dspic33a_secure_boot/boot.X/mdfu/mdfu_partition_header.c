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

#include <stdbool.h>
#include "mdfu_partition_header.h"
#include "mdfu_config.h"

#define TYPE_LENGTH     4U
#define LENGTH_LENGTH   4U

#define TYPE_OFFSET     0U
#define LENGTH_OFFSET   (TYPE_LENGTH)
#define VALUE_OFFSET    (TYPE_LENGTH + LENGTH_LENGTH)

#define END_OF_HEADER_OFFSET   (MDFU_CONFIG_MAX_HEADER_LENGTH + MDFU_CONFIG_HEADER_OFFSET)

/**
 * @brief    Determines if we have reached the end of the header yet
 * @param    const uint32_t offset - the current image offset
 * @param    const enum MDFU_PARTITION_HEADER type - the type of the last entry read
 * @return   bool - true if we have reached the end of the header, false otherwise
 */
static bool IsEndOfHeader(const uint32_t offset, const enum MDFU_PARTITION_HEADER type)
{
    bool endOfHeader = false;
    
    if((MDFU_PARTITION_HEADER_END_OF_HEADER == type) ||
       (offset >= END_OF_HEADER_OFFSET))
    {
        endOfHeader = true;
    }
    
    return endOfHeader;
}

/**
 * @brief    returns the min of the two inputs
 * @param    uint32_t a - first input
 * @param    uint32_t b - second input
 * @return   uint32_t - the lesser of the two inputs
 */
static inline uint32_t Min(const uint32_t a, const uint32_t b)
{
    return (a < b) ? a : b;
}

/**
 * @brief    Scans partition header for requested entry
 * @param    const struct PARTITION* partition - pointer to the partition to search
 * @param    enum MDFU_PARTITION_HEADER code - the type code of item to find
 * @param    size_t requestedLength - the amount of data to read, if found
 * @param    void* buffer - buffer where to read the requested data
 * @return   size_t - amount of data read, 0 if the item was not found
 */
size_t MDFU_HeaderItemBufferRead(const struct MDFU_PARTITION *partition, const enum MDFU_PARTITION_HEADER code, const size_t requestedLength, void* buffer)
{
    enum MDFU_PARTITION_HEADER type;
    uint32_t length = 0;
    size_t dataRead = 0;
    
    if((NULL != partition) &&
       (NULL != buffer))
    {
        uint32_t entryOffset = MDFU_CONFIG_HEADER_OFFSET;
        
        do
        {
            partition->read(entryOffset + TYPE_OFFSET, sizeof(enum MDFU_PARTITION_HEADER), &type);
            partition->read(entryOffset + LENGTH_OFFSET, LENGTH_LENGTH, &length);
            
            if(code == type)
            {
                length = Min(requestedLength, length);
                
                dataRead = partition->read(entryOffset + VALUE_OFFSET, length, buffer);
                break;
            }

            entryOffset += VALUE_OFFSET + length;
        } while (IsEndOfHeader(entryOffset, type) == false);
    }
    
    return dataRead;
}