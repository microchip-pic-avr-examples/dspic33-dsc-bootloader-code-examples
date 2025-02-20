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

#include <string.h>
#include <stdint.h>

#include <xc.h>

static size_t Read(uint32_t offset, size_t length, void* buffer);
static enum PARTITION_STATUS ModeChange (int mode);

const struct PARTITION application = 
{
    .read = &Read,
    .modeChange = &ModeChange
};

/**
 * @brief    Reads the specified memory from the application partition
 * @param    uint32_t offset - offset into the partition to start reading from
 * @param    size_t length - amount of data to read from partition
 * @param    void* buffer - pointer to where to write the data that is read
 * @return   size_t - the amount of data actually read
 */
static size_t Read(uint32_t offset, size_t length, void* buffer)
{
    /* [misra-c2012-11.4] Advisory - A conversion should not be performed 
     * between a pointer to object and an integer type.
     * 
     * This is required.  The code needs to know the device address where the
     * partition is located.  Since this is a fixed address and not a variable,
     * it requires a cast.  The alternative would be to create a dummy variable
     * at the address of the partition start and point to that instead.  This
     * alternative would take a custom addressed variable and corresponding
     * considerations in the linker file.
     */
    const uint8_t* applicationStart = (const uint8_t*)MDFU_CONFIG_APPLICATION_ADDRESS_LOW;
    
    size_t actualLength = length;
            
    (void)memcpy(buffer, &applicationStart[offset], actualLength);
    
    return actualLength;
}

/**
 * @brief    Changes the mode of operation of the partition
 * @param    enum PARTITION_MODE mode - the mode of operation to set the drive 
 *           into.  This is the logical AND of all of the modes required (of type 
 *           enum PARTITION_MODE).  For example:
 *               ModeChange (PARTITION_MODE_READ & PARTITION_MODE_WRITE);
 * @return   enum PARTITION_STATUS - status of the requested mode change
 */
enum PARTITION_STATUS ModeChange(enum PARTITION_MODE mode)
{
    enum PARTITION_STATUS status = PARTITION_STATUS_MODE_UNSUPPORTED;
    
    if(PR1LOCKbits.LOCK == 0b01)
    {
        status = PARTITION_STATUS_LOCKED;
    } 
    else if(mode == (PARTITION_MODE_READ & PARTITION_MODE_WRITE))
    {
        
        PR1LOCK = 0xB7370003UL; //Unlock
        PR1CTRLbits.WR = 1;
        PR1CTRLbits.RD = 1;
        PR1CTRLbits.EX = 0;
        PR1LOCK = 0xB7370000UL; //Locked but can be unlocked again
        
        status = PARTITION_STATUS_SUCCESS;
    } 
    else if(mode == (PARTITION_MODE_EXECUTABLE & PARTITION_MODE_READ & PARTITION_MODE_LOCKED))
    {
        PR1LOCK = 0xB7370003UL; //Unlock
        PR1CTRLbits.WR = 0;
        PR1CTRLbits.RD = 1;
        PR1CTRLbits.EX = 1;
        PR1LOCK = 0xB7370001UL; //Locked until next rest
        
        status = PARTITION_STATUS_SUCCESS;
    }
    
    return status;
}