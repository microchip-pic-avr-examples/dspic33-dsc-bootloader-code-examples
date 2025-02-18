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

#ifndef MDFU_PARTITION
#define MDFU_PARTITION

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

enum PARTITION_MODE {
    PARTITION_MODE_READ =         ~(1<<0),
    PARTITION_MODE_WRITE =        ~(1<<1),
    PARTITION_MODE_EXECUTABLE =   ~(1<<2),
    PARTITION_MODE_LOCKED =       ~(1<<3)
};

enum PARTITION_STATUS {
    PARTITION_STATUS_SUCCESS = 0,
    PARTITION_STATUS_MODE_UNSUPPORTED,
    PARTITION_STATUS_LOCKED,
};

struct PARTITION
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
     * @brief    Changes the mode of operation of the partition
     * @param    enum PARTITION_MODE mode - the mode of operation to set the drive 
     *           into.  This is the logical AND of all of the modes required (of type 
     *           enum PARTITION_MODE).  For example:
     *               ModeChange (PARTITION_MODE_READ & PARTITION_MODE_WRITE);
     * @return   enum PARTITION_STATUS - status of the requested mode change
     */
    enum PARTITION_STATUS (*modeChange)(enum PARTITION_MODE mode);
};

#endif