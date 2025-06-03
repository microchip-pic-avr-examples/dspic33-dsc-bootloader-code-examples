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

#ifndef MDFU_PARTITION_H
#define MDFU_PARTITION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

enum MDFU_PARTITION_MODE {
    MDFU_PARTITION_MODE_READ =         ~(1U<<0U),
    MDFU_PARTITION_MODE_WRITE =        ~(1U<<1U),
    MDFU_PARTITION_MODE_EXECUTABLE =   ~(1U<<2U),
    MDFU_PARTITION_MODE_LOCKED =       ~(1U<<3U)
};

enum MDFU_PARTITION_STATUS {
    MDFU_PARTITION_STATUS_SUCCESS = 0,
    MDFU_PARTITION_STATUS_MODE_UNSUPPORTED,
    MDFU_PARTITION_STATUS_LOCKED,
    MDFU_PARTITION_STATUS_INVALID_ARGUMENT,
    MDFU_PARTITION_STATUS_OPERATION_FAILED
};

enum MDFU_PARTITION_HASH_ALGORITHM {
    MDFU_PARTITION_HASH_ALGORITHM_SHA2_384
};

struct MDFU_PARTITION
{
    /**
     * @brief    Reads the specified memory from the boot partition
     * @param    uint32_t offset - offset into the image to start reading from
     * @param    size_t length - amount of data to read from image
     * @param    void* buffer - pointer to where to write the data that is read
     * @return   size_t - the amount of data actually read
     */
    size_t (*read)(uint32_t offset, size_t length, void* buffer);
        
    /**
     * @brief    Writes data to the specified address in memory. This is to be called by the client's MDFU Firmware Update Layer when 
     *           a write chunk command has been received from the MDFU Command Processor Layer
     * @param    uint32_t address - offset of the write operation from the partition root 
     * @param    size_t length - Number of bytes to be written
     * @param    uint8_t const * const - Pointer to the buffer containing data to be written
     * @return   Status indicating if the write operation was successful and if not, what error occurred 
     */
    enum MDFU_PARTITION_STATUS (*write)(uint32_t offset, size_t length, uint8_t const * const data);
        
    /**
     * @brief    Erases the entire partition space in preparation for write operations. This is to be called by the client's 
     *           MDFU Firmware Update Layer when a start transfer command has been received from the MDFU Command Processor Layer
     * @return   Status indicating if the erase operation was successful
     */
    enum MDFU_PARTITION_STATUS (*erase)(void);
    
     /**
     * @brief    Copy takes the content of the source partition and copies it into this partition.
     * @param    struct MDFU_PARTITION const * const - Pointer to the partition to be copied
     * @return   Status indicating if the copy operation was successful
     */
    enum MDFU_PARTITION_STATUS (*copy)(const struct MDFU_PARTITION* source);
    
    /**
     * @brief    Run will attempt to switch from the current bootloader to running the image stored 
     *           in this partition. 
     * @return   Status indicating if the run failed.
     */
    enum MDFU_PARTITION_STATUS (*run)(void);
    
    /**
     * @brief    Changes the mode of operation of the partition
     * @param    enum PARTITION_MODE mode - the mode of operation to set the drive 
     *           into.  This is the logical AND of all of the modes required (of type 
     *           enum PARTITION_MODE).  For example:
     *               ModeChange (PARTITION_MODE_READ & PARTITION_MODE_WRITE);
     * @return   enum PARTITION_STATUS - status of the requested mode change
     */
    enum MDFU_PARTITION_STATUS (*modeChange)(enum MDFU_PARTITION_MODE mode);
    
    /**
     * @brief      Hashes the requested block of partition memory
     * @param[in]  enum MDFU_PARTITION_HASH_ALGORITHM algorithm - the hash
     *             algorithm to use.  
     *           
     *             If the algorithm passes it is not supported by the partition, 
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
    enum MDFU_PARTITION_STATUS (*hash)(enum MDFU_PARTITION_HASH_ALGORITHM algorithm, uint32_t offset, uint32_t length, uint8_t *digest);
};

#endif