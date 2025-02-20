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
#include <stdint.h>
#include <stdbool.h>

#include "mdfu_verification.h"
#include "mdfu_application_header.h"
#include "mdfu_config.h"
#include "mdfu_partition.h"
#include "crc32q.h"

/**
 * @brief    Calcuates the CRC of a specified range of an image
 * @param    struct IMAGE* image - the image to read from
 * @param    const uint32_t startOffset - the offset in the image to start from
 * @param    const uint32_t length - the amount of data to read as CRC input
 * @return   uint32_t - the CRC32Q value of the specified data.
 */
static uint32_t PartitionCrcCalculate(struct PARTITION const * const partition, const uint32_t startOffset, uint32_t length)
{   
    uint32_t crc = 0;
    
    for(uint32_t i=0; i<length; i++)
    {    
        uint8_t data;
        
        partition->read(startOffset+i, 1, &data);
        
        crc = crc32q(crc, &data, 1);
    }
    
    return crc;
}

/**
 * @brief    Verifies the validity of the partition header
 * @param    struct PARTITION* partition - the partition with header to verify
 * @return   enum MDFU_VERIFY_CODE - code of result. MDFU_VERIFY_CODE_SUCCESS if 
 *           no errors are detected.
 */
static enum MDFU_VERIFY_CODE VerifyHeader(struct PARTITION const * const partition)
{
    enum MDFU_VERIFY_CODE returnCode = MDFU_VERIFY_CODE_SUCCESS;  
    uint32_t headerCrcCalculated = 0;
    uint32_t headerCrcExpected = 0;

    //Read the expected CRC of the header
    partition->read(0, sizeof(uint32_t), &headerCrcExpected);

    //Calculate the actual CRC of the header
    headerCrcCalculated = PartitionCrcCalculate(partition, MDFU_CONFIG_HEADER_OFFSET, MDFU_CONFIG_MAX_HEADER_LENGTH);
        
    if(headerCrcExpected != headerCrcCalculated)
    {
        returnCode = MDFU_VERIFY_CODE_INVALID_HEADER;
    }
    
    return returnCode;
}

/**
 * @brief    Verifies the integrity of the partition
 * @param    struct PARTITION* partition - the partition with header to verify
 * @return   enum MDFU_VERIFY_CODE - code of result. MDFU_VERIFY_CODE_SUCCESS if 
 *           no errors are detected.
 */
static enum MDFU_VERIFY_CODE VerifyPartition(struct PARTITION const * const partition)
{
    enum MDFU_VERIFY_CODE returnCode = MDFU_VERIFY_CODE_INVALID_HEADER;
    uint32_t expectedCrc = 0;
    uint32_t codeSize = 0;
    bool headerError = VerifyHeader(partition);
    
    if(MDFU_AppHeaderItemBufferRead(partition, MDFU_APPLICATION_HEADER_CODE_INTEGRITY, sizeof(uint32_t), &expectedCrc) != sizeof(uint32_t))
    {
        headerError = true;
    }
    
    if(MDFU_AppHeaderItemBufferRead(partition, MDFU_APPLICATION_HEADER_CODE_SIZE, sizeof(uint32_t), &codeSize) != sizeof(uint32_t))
    {
        headerError = true;
    }

    if( false == headerError )
    {
        uint32_t calculatedCrc = PartitionCrcCalculate(partition, MDFU_CONFIG_CODE_OFFSET, codeSize);

        if(calculatedCrc == expectedCrc) 
        {
            returnCode = MDFU_VERIFY_CODE_SUCCESS;
        }
        else
        {
            returnCode = MDFU_VERIFY_CODE_INVALID_INTEGRITY_CHECK;
        }
    }
    
    return returnCode;
}

/**
 * @brief    Verifies the header and partition data of the provided partition
 * @param    struct PARTITION* partition - the partition with header to verify
 * @return   enum MDFU_VERIFY_CODE - code of result. MDFU_VERIFY_CODE_SUCCESS if 
 *           no errors are detected.
 */
enum MDFU_VERIFY_CODE MDFU_Verify(struct PARTITION const * const partition) 
{        
    enum MDFU_VERIFY_CODE result = MDFU_VERIFY_CODE_INVALID_PARTITION;
        
    if(partition != NULL){
        result = VerifyPartition(partition);
    }

    return result;
}