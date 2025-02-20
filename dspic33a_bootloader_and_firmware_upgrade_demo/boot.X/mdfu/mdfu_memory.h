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
#ifndef MDFU_MEMORY_H
#define MDFU_MEMORY_H

#include <stdint.h>

enum MDFU_MEMORY_RESULT
{
    MDFU_MEMORY_PASS,
    MDFU_MEMORY_FAIL,
    MDFU_MEMORY_WRITE_ERROR,
    MDFU_MEMORY_INVALID_ADDRESS,
    MDFU_MEMORY_INVALID_LENGTH,
};

/**
 * @brief    Writes data to the specified address in memory. This is to be called by the client's MDFU Firmware Update Layer when 
 *           a write chunk command has been received from the MDFU Command Processor Layer.    
 * @param [in]  address - Starting address for the write operation 
 * @param [in]  length - Number of bytes to be written
 * @param [in]  data - Buffer containing the data to be written 
 * @param [in]  key - Unlock sequence to unlock memory for write operation
 * @return   Status indicating if the write operation was successful and if not, what error occurred  
 */
enum MDFU_MEMORY_RESULT MDFU_MemoryWriteChunk(uint32_t address, size_t length, uint8_t const * const data, uint32_t key);

/**
 * @brief    Erases the entire application space in preparation for write operations. This is to be called by the client's 
 *           MDFU Firmware Update Layer when a start transfer command has been received from the MDFU Command Processor Layer.  
 * @param [in]  key - Unlock sequence to unlock memory for erase operation
 * @return   Status indicating if the erase operation was successful or not.   
 */
enum MDFU_MEMORY_RESULT MDFU_MemoryEraseApplication(uint32_t key);

#endif