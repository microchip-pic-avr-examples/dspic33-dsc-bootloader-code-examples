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
#include "../bsp/flash_region.h"
#include "../bsp/flash_region0.h"
#include "mdfu_partition_boot.h"

#include <string.h>
#include <stdint.h>

#include <xc.h>

static size_t Read(uint32_t offset, size_t length, void* buffer);
static enum MDFU_PARTITION_STATUS ModeChange(enum MDFU_PARTITION_MODE mode);
static enum MDFU_PARTITION_STATUS Write(uint32_t address, size_t length, uint8_t const * const data);
static enum MDFU_PARTITION_STATUS Erase(void);
static enum MDFU_PARTITION_STATUS Copy(const struct MDFU_PARTITION* source);
static enum MDFU_PARTITION_STATUS Run(void);

const struct MDFU_PARTITION boot = {
    .read = &Read,
    .modeChange = &ModeChange,
    .write = &Write,
    .erase = &Erase,
    .copy = &Copy,
    .run = &Run,
};

/**
 * @brief    Writes data to the specified address in memory. This is to be called by the client's MDFU Firmware Update Layer when 
 *           a write chunk command has been received from the MDFU Command Processor Layer
 * @param    uint32_t address - Starting address for the write operation 
 * @param    size_t length - Number of bytes to be written
 * @param    uint8_t const * const - Pointer to the buffer containing data to be written
 * @return   Status indicating if the write operation was successful and if not, what error occurred 
 */
static enum MDFU_PARTITION_STATUS Write(uint32_t address, size_t length, uint8_t const * const data)
{
    /* Unused parameters */
    (void)address;
    (void)length;
    (void)data;
    
    return MDFU_PARTITION_STATUS_OPERATION_FAILED;
}

/**
 * @brief    Erases the entire partition space in preparation for write operations. This is to be called by the client's 
 *           MDFU Firmware Update Layer when a start transfer command has been received from the MDFU Command Processor Layer
 * @return   Status indicating if the erase operation was successful
 */
static enum MDFU_PARTITION_STATUS Erase(void)
{
    return MDFU_PARTITION_STATUS_OPERATION_FAILED;
}

/**
 * @brief    Reads the specified memory from the boot partition
 * @param    uint32_t offset - offset into the image to start reading from
 * @param    size_t length - amount of data to read from image
 * @param    void* buffer - pointer to where to write the data that is read
 * @return   size_t - the amount of data actually read
 */
static size_t Read(uint32_t offset, size_t length, void* buffer)
{
    /* Unused parameters */
    (void)offset;
    (void)length;
    (void)buffer;
    
    return 0;
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
 * @param    enum MDFU_PARTITION_MODE mode - the mode of operation to set the drive 
 *           into.  This is the logical AND of all of the modes required (of type 
 *           enum MDFU_PARTITION_MODE).  For example:
 *               ModeChange (MDFU_PARTITION_MODE_READ & MDFU_PARTITION_MODE_WRITE);
 * @return   enum MDFU_PARTITION_STATUS - status of the requested mode change
 */
static enum MDFU_PARTITION_STATUS ModeChange(enum MDFU_PARTITION_MODE mode)
{
    enum MDFU_PARTITION_STATUS status = MDFU_PARTITION_STATUS_MODE_UNSUPPORTED;
    
    if (mode == (enum MDFU_PARTITION_MODE)(MDFU_PARTITION_MODE_EXECUTABLE & MDFU_PARTITION_MODE_READ & MDFU_PARTITION_MODE_LOCKED)) 
    {
        enum FLASH_REGION_PERMISSION permissions = (FLASH_REGION_PERMISSION_EXECUTION_ENABLED & FLASH_REGION_PERMISSION_READ_ENABLED);
        status = flashRegion0.permissionsSet(permissions, true) ? MDFU_PARTITION_STATUS_SUCCESS : MDFU_PARTITION_STATUS_LOCKED;
    } 
    else 
    {
    }
    
    return status;
}