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

#include "mdfu_partition_keystore.h"
#include "mdfu_internal_flash.h"
#include "mdfu_config.h"
#include "../bsp/flash_region1.h"

#include <string.h>
#include <stdint.h>

#include <xc.h>

#define P384_PUBLIC_KEY_SIZE    97UL

static size_t Read(uint32_t offset, size_t length, void* buffer);
static enum MDFU_PARTITION_STATUS Write(uint32_t address, size_t length, uint8_t const * const data);
static enum MDFU_PARTITION_STATUS Erase(void);
static enum MDFU_PARTITION_STATUS Copy(const struct MDFU_PARTITION* source);
static enum MDFU_PARTITION_STATUS Run(void);
static enum MDFU_PARTITION_STATUS ModeChange (enum MDFU_PARTITION_MODE mode);
static enum MDFU_PARTITION_STATUS Hash(enum MDFU_PARTITION_HASH_ALGORITHM algorithm, uint32_t offset, uint32_t length, uint8_t *digest);

const struct MDFU_PARTITION keystore = {
    .read = &Read,
    .write = &Write,
    .erase = &Erase,
    .copy = &Copy,
    .run = &Run,
    .modeChange = &ModeChange,
    .hash = &Hash
};

/**
 * @brief    Reads the specified memory from the keystore partition
 * @param    uint32_t offset - offset into the partition to start reading from
 * @param    size_t length - amount of data to read from partition
 * @param    void* buffer - pointer to where to write the data that is read
 * @return   size_t - the amount of data actually read
 */
static size_t Read(uint32_t offset, size_t length, void* buffer)
{    
    size_t actualLength = length;
            
    if(((MDFU_CONFIG_KEYSTORE_ORIGIN + offset + length) > (MDFU_CONFIG_KEYSTORE_ORIGIN + P384_PUBLIC_KEY_SIZE) ) ||
       (NULL == buffer))
    {
        actualLength = 0;
    } else {
        /* cppcheck-suppress misra-c2012-11.4
         * 
         *  (Rule 11.4) ADVISORY: A conversion should not be performed between a
         *  pointer to object and an integer type
         * 
         *  Reasoning: The keystore is a binary object of data generated into an
         *  assembly file which contains no label to use for reference to.  
         *  Parsing of the keystore object happens at the start of the assigned
         *  keystore storage address.
         */
        const uint8_t* publicKeyStart = (const uint8_t*)MDFU_CONFIG_KEYSTORE_DATA_ORIGIN;
        
        (void)memcpy(buffer, &publicKeyStart[offset], actualLength);
    }
    
    return actualLength;
}

/**
 * @brief    Write to the keystore is not currently supported.
 * @param    uint32_t offset - offset for the write operation.  Must be aligned to hardware write requirements.
 * @param    size_t length - Number of bytes to be written
 * @param    uint8_t const * const - Pointer to the buffer containing data to be written
 * @return   Always returns MDFU_PARTITION_STATUS_OPERATION_FAILED
 */
static enum MDFU_PARTITION_STATUS Write(uint32_t offset, size_t length, uint8_t const * const data)
{
    /* Unused parameters */
    (void)offset;
    (void)length;
    (void)data;
    
    return MDFU_PARTITION_STATUS_OPERATION_FAILED;
}

/**
 * @brief    Erase of the keystore is not allowed.
 * @return   Always returns MDFU_PARTITION_STATUS_OPERATION_FAILED
 */
static enum MDFU_PARTITION_STATUS Erase(void)
{
    return MDFU_PARTITION_STATUS_OPERATION_FAILED;
}

/**
* @brief    Copy not implemented for this partition.
* @param    struct MDFU_PARTITION const * const - Pointer to the partition to be copied
* @return   Status indicating if the copy operation was successful
*/
static enum MDFU_PARTITION_STATUS Copy(const struct MDFU_PARTITION* source)
{    
    /* Unused parameters */
    (void)source;
    
    return MDFU_PARTITION_STATUS_OPERATION_FAILED;
}

/**
* @brief    Run not implemented for this partition.
* @param    struct MDFU_PARTITION const * const - Pointer to the partition to be copied
* @return   Status indicating if the copy operation was successful
*/
static enum MDFU_PARTITION_STATUS Run(void)
{    
    return MDFU_PARTITION_STATUS_OPERATION_FAILED;
}

/**
 * @brief    Changes the mode of operation of the partition
 * @param    int mode - the mode of operation to set the drive into.  This is
 *           the logical OR of all of the modes required (of type 
 *           enum MDFU_PARTITION_MODE).  For example:
 *               ModeChange (MDFU_PARTITION_MODE_READ | MDFU_PARTITION_MODE_WRITE);
 * @return   enum MDFU_PARTITION_STATUS - status of the requested mode change
 */
enum MDFU_PARTITION_STATUS ModeChange(enum MDFU_PARTITION_MODE mode) 
{
    enum MDFU_PARTITION_STATUS status = MDFU_PARTITION_STATUS_MODE_UNSUPPORTED;
    
    if (mode == MDFU_PARTITION_MODE_LOCKED) 
    {
        /* Default permissions */
        enum FLASH_REGION_PERMISSION permissions = (FLASH_REGION_PERMISSION_READ_ENABLED);
        status = flashRegion1.permissionsSet(permissions, true) ? MDFU_PARTITION_STATUS_SUCCESS : MDFU_PARTITION_STATUS_LOCKED;
    }
    else
    {
    }

    return status;
}

/**
 * @brief    Hash of the keystore is not currently supported
 * @return   Always returns MDFU_PARTITION_STATUS_OPERATION_FAILED
 */
static enum MDFU_PARTITION_STATUS Hash(enum MDFU_PARTITION_HASH_ALGORITHM algorithm, uint32_t offset, uint32_t length, uint8_t *digest)
{
    /* Unused parameters */
    (void)algorithm;
    (void)offset;
    (void)length;
    (void)digest;
    
    return MDFU_PARTITION_STATUS_OPERATION_FAILED;
}