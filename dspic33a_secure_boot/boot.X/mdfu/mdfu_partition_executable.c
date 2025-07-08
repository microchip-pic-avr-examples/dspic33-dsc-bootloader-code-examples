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
#include "mdfu_partition_executable.h"
#include "mdfu_internal_flash.h"
#include "mdfu_config.h"
#include "crypto/common_crypto/crypto_common.h"
#include "crypto/common_crypto/crypto_hash.h"
#include "../bsp/flash_region2.h"
#include "../mcc_generated_files/system/interrupt.h"

#include <string.h>
#include <stdint.h>

#include <xc.h>

#define MAX_COPY_BUFFER_LENGTH MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH

static size_t Read(uint32_t offset, size_t length, void* buffer);
static enum MDFU_PARTITION_STATUS Write(uint32_t address, size_t length, uint8_t const * const data);
static enum MDFU_PARTITION_STATUS Erase(void);
static enum MDFU_PARTITION_STATUS Copy(const struct MDFU_PARTITION* source);
static enum MDFU_PARTITION_STATUS Run(void);
static enum MDFU_PARTITION_STATUS ModeChange(enum MDFU_PARTITION_MODE mode);
static enum MDFU_PARTITION_STATUS Hash(enum MDFU_PARTITION_HASH_ALGORITHM algorithm, uint32_t offset, uint32_t length, uint8_t *digest);

const struct MDFU_PARTITION executable = {
    .read = &Read,
    .write = &Write,
    .erase = &Erase,
    .copy = &Copy,
    .run = &Run,
    .modeChange = &ModeChange,
    .hash = &Hash
};

/**
 * @brief    Reads the specified memory from the executable partition
 * @param    uint32_t offset - offset into the partition to start reading from
 * @param    size_t length - amount of data to read from partition
 * @param    void* buffer - pointer to where to write the data that is read
 * @return   size_t - the amount of data actually read
 */
static size_t Read(uint32_t offset, size_t length, void* buffer)
{
    /* cppcheck-suppress misra-c2012-11.4
     * 
     *  (Rule 11.4) ADVISORY: A conversion should not be performed 
     * between a pointer to object and an integer type.
     * 
     *  Reasoning: This is required.  The code needs to know the device address 
     *  where the partition is located.  Since this is a fixed address and not a
     *  variable, it requires a cast.  The alternative would be to create a 
     *  dummy variable at the address of the partition start and point to that 
     *  instead.  This alternative would take a custom addressed variable and 
     *  corresponding considerations in the linker file.
     */
    const uint8_t* executableStart = (const uint8_t*)MDFU_CONFIG_EXECUTABLE_ORIGIN;
    
    size_t actualLength = length;
            
    if(((MDFU_CONFIG_EXECUTABLE_ORIGIN + offset + length) > (MDFU_CONFIG_EXECUTABLE_ORIGIN + MDFU_CONFIG_EXECUTABLE_LENGTH)) ||
       (NULL == buffer))
    {
        actualLength = 0;
    } else {
        (void)memcpy(buffer, &executableStart[offset], actualLength);
    }
    
    return actualLength;
}

/**
 * @brief    Writes data to the specified address in memory. This is to be called by the client's MDFU Firmware Update Layer when 
 *           a write chunk command has been received from the MDFU Command Processor Layer
 * @param    uint32_t offset - offset for the write operation.  Must be aligned to hardware write requirements.
 * @param    size_t length - Number of bytes to be written
 * @param    uint8_t const * const - Pointer to the buffer containing data to be written
 * @return   Status indicating if the write operation was successful and if not, what error occurred 
 */
static enum MDFU_PARTITION_STATUS Write(uint32_t offset, size_t length, uint8_t const * const data)
{
    enum MDFU_PARTITION_STATUS status = MDFU_PARTITION_STATUS_SUCCESS;
    
    const uint32_t startAddress = MDFU_CONFIG_EXECUTABLE_ORIGIN + offset;
    const uint32_t endAddress = startAddress + length;

    if((NULL == data) ||
       (startAddress < MDFU_CONFIG_EXECUTABLE_ORIGIN) ||
       (endAddress > (MDFU_CONFIG_EXECUTABLE_ORIGIN + MDFU_CONFIG_EXECUTABLE_LENGTH)))
    {
        status = MDFU_PARTITION_STATUS_INVALID_ARGUMENT;
    } 
    else if(MDFU_INTERNAL_FLASH_Write(MDFU_CONFIG_EXECUTABLE_ORIGIN + offset, length, data) == false)
    {
        status = MDFU_PARTITION_STATUS_OPERATION_FAILED; 
    }
    else
    {
    }

    return status;
}

/**
 * @brief    Erases the entire executable partition in preparation for write operations. This is to be called by the client's 
 *           MDFU Firmware Update Layer when a start transfer command has been received from the MDFU Command Processor Layer
 * @return   Status indicating if the erase operation was successful
 */
static enum MDFU_PARTITION_STATUS Erase(void)
{
    enum MDFU_PARTITION_STATUS status = MDFU_PARTITION_STATUS_SUCCESS;
    
    if(MDFU_INTERNAL_FLASH_BlockErase(MDFU_CONFIG_EXECUTABLE_ORIGIN, MDFU_CONFIG_EXECUTABLE_LENGTH) == false)
    { 
        status = MDFU_PARTITION_STATUS_OPERATION_FAILED; 
    }
    
    return status;
}

/**
* @brief    Copy takes the content of the source partition and copies it into this partition.
* @param    struct MDFU_PARTITION const * const - Pointer to the partition to be copied
* @return   Status indicating if the copy operation was successful
*/
static enum MDFU_PARTITION_STATUS Copy(const struct MDFU_PARTITION* source)
{
    enum MDFU_PARTITION_STATUS status = MDFU_PARTITION_STATUS_OPERATION_FAILED;
    uint32_t copyOffset = 0;
    uint8_t copyBuffer[MAX_COPY_BUFFER_LENGTH];
    uint32_t readSize = MAX_COPY_BUFFER_LENGTH;
         
    if(source == NULL)
    {
        status = MDFU_PARTITION_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        status = Erase();
        while((status == MDFU_PARTITION_STATUS_SUCCESS) && 
              (copyOffset < MDFU_CONFIG_EXECUTABLE_LENGTH) &&
              (readSize > 0UL))
        {
            readSize = source->read(copyOffset, readSize, &copyBuffer);
            if(readSize > 0UL)
            {
                status = Write(copyOffset, readSize, copyBuffer);
                copyOffset = copyOffset + readSize;
            }
        }
    }
    
    // Check if full copy completed successfully
    if((status == MDFU_PARTITION_STATUS_SUCCESS) &&
       (copyOffset != MDFU_CONFIG_EXECUTABLE_LENGTH))
    {
        status = MDFU_PARTITION_STATUS_OPERATION_FAILED;
    }
    
    return status;
}

static void CacheInvalidate(void)
{
    //Disable cache   
    CHECONbits.ON = 0;  
    
    //Force cache invalidation just in case
    CHECONbits.CHEINV = 1;
    while(CHECONbits.CHEINV == 1){}
}

/**
* @brief    Run not implemented for this partition.
* @param    struct MDFU_PARTITION const * const - Pointer to the partition to be copied
* @return   Status indicating if the copy operation was successful
*/
static enum MDFU_PARTITION_STATUS Run(void)
{    
     /* NOTE: Before starting the executable, all interrupts used
    * by the bootloader must be disabled. Add code here to return
    * the peripherals/interrupts to the reset state before starting
    * the executable code. */
    #warning "All interrupt sources and peripherals should be disabled before starting the executable. Add any code required here to disable all interrupts and peripherals used in the bootloader."
    INTERRUPT_GlobalDisable();
    INTERRUPT_Deinitialize();
    
    CacheInvalidate();
    
    /* This is probably being replaced at some point by __builtin__setIVTBASE() but is not currently supported */
    /* This also assumes that the first block of memory in the executable partition is the reset/ivt table which may not always be true if there is an executable header */
    PACCON1bits.IVTBASEWR = 1;
    IVTBASE = MDFU_CONFIG_EXECUTABLE_DATA_ORIGIN;
    PACCON1bits.IVTBASEWR = 0;
    
    /* cppcheck-suppress misra-c2012-11.6
     * 
     *  (Rule 11.6) REQUIRED: Required: A cast shall not be performed between 
     *  pointer to void and an arithmetic type
     * 
     *  Reasoning: This function represents a jump between the boot code and the
     *  user code. Because the address of the jump lives outside of boot space,
     *  there is no way to create an object at that address to references so
     *  an integer address is used for the pre-defined executable entry point.
     */
    int (*user_executable)(void);
    
    uint32_t *resetVectorPtr = (uint32_t *)MDFU_CONFIG_EXECUTABLE_DATA_ORIGIN;
    uint32_t resetVector = *resetVectorPtr;

    user_executable = (int(*)(void))resetVector;
    
     /* Disable IRT access before transferring control to the executable.
      * 
      * NOTE: The datasheet advises that the IRT should not fetch memory from a non-IRT 
      * section before the DONE bit is set. To achieve this, it recommends placing 
      * at least a 32-byte buffer between the executable IRT section and any 
      * executable non-IRT sections. The keystore, although classified as IRT, is 
      * non-executable and thus serves as the recommended buffer. */
    IRTCTRLbits.DONE = 1U;
    user_executable();  
    
    return MDFU_PARTITION_STATUS_OPERATION_FAILED;
}

/**
 * @brief    Changes the mode of operation of the partition
 * @param    enum MDFU_PARTITION_MODE mode - the mode of operation to set the drive 
 *           into.  This is the logical AND of all of the modes required (of type 
 *           enum MDFU_PARTITION_MODE).  For example:
 *               ModeChange (MDFU_PARTITION_MODE_READ & MDFU_PARTITION_MODE_WRITE);
 * @return   enum MDFU_PARTITION_STATUS - status of the requested mode change
 */
static enum MDFU_PARTITION_STATUS ModeChange(enum MDFU_PARTITION_MODE mode)
{
    enum MDFU_PARTITION_STATUS status = MDFU_PARTITION_STATUS_MODE_UNSUPPORTED;

    if (mode == (enum MDFU_PARTITION_MODE)(MDFU_PARTITION_MODE_READ & MDFU_PARTITION_MODE_WRITE))
    {
        enum FLASH_REGION_PERMISSION permissions = (FLASH_REGION_PERMISSION_READ_ENABLED & FLASH_REGION_PERMISSION_WRITE_ENABLED);
        status = flashRegion2.permissionsSet(permissions, false) ? MDFU_PARTITION_STATUS_SUCCESS : MDFU_PARTITION_STATUS_LOCKED;
    } 
    else if (mode == (enum MDFU_PARTITION_MODE)(MDFU_PARTITION_MODE_EXECUTABLE & MDFU_PARTITION_MODE_READ & MDFU_PARTITION_MODE_LOCKED))
    {
        enum FLASH_REGION_PERMISSION permissions = (FLASH_REGION_PERMISSION_EXECUTION_ENABLED & FLASH_REGION_PERMISSION_READ_ENABLED);
        status = flashRegion2.permissionsSet(permissions, true) ? MDFU_PARTITION_STATUS_SUCCESS : MDFU_PARTITION_STATUS_LOCKED;
    }
    else
    {
    }
    
    return status;
}

/**
 * @brief      Hashes the requested block of partition memory
 * @param[in]  enum MDFU_PARTITION_HASH_ALGORITHM algorithm - the hash
 *             algorithm to use.  
 *           
 *             If the algorithm passed is not supported by the partition, 
 *             the function will return MDFU_PARTITION_STATUS_INVALID_ARGUMENT
 *           
 *             If there was an error during operation, the function will
 *             return MDFU_PARTITION_STATUS_OPERATION_FAILED
 * @param[in]  uint32_t offset - the starting offset where to start the hash
 * @param[in]  uint32_t length - amount of data to hash
 * @param[out] uint8_t *digest - the output buffer where to store the
 *             resulting digest from the hash operation.  
 * 
 * @return     enum PARTITION_STATUS - status result of the operation
 */
static enum MDFU_PARTITION_STATUS Hash(enum MDFU_PARTITION_HASH_ALGORITHM algorithm, uint32_t offset, uint32_t length, uint8_t *digest)
{
    enum MDFU_PARTITION_STATUS returnCode = MDFU_PARTITION_STATUS_OPERATION_FAILED;
    
    if ((algorithm != MDFU_PARTITION_HASH_ALGORITHM_SHA2_384) || (digest == NULL))
    {
        returnCode = MDFU_PARTITION_STATUS_INVALID_ARGUMENT;
    }
    else 
    {
        /* cppcheck-suppress misra-c2012-11.4
         * 
         *  (Rule 11.4) ADVISORY: A conversion should not be performed between a
         *  pointer to object and an integer type
         * 
         *  Reasoning: This file maps to internal memory to read/hash from.  The
         *  flash memory can be directly accessed by the crypto module without
         *  needing to read it into a RAM buffer first.  This saves a lot of RAM
         *  and CPU time but requires us to create a pointer that points directly
         *  to the flash memory from its address.
         */
        uint8_t* hashAddress = (uint8_t*)(MDFU_CONFIG_EXECUTABLE_ORIGIN + offset); 
        
        if (Crypto_Hash_Sha_Digest(CRYPTO_HANDLER_HW_INTERNAL, hashAddress, length, digest, CRYPTO_HASH_SHA2_384, 1U) == CRYPTO_HASH_SUCCESS)
        {
            returnCode = MDFU_PARTITION_STATUS_SUCCESS;
        }
    }
    
    return returnCode;
}