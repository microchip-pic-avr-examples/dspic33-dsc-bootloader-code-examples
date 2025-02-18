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
#ifndef MDFU_FIRMWARE_UPDATE_H
#define MDFU_FIRMWARE_UPDATE_H

#include <stdint.h>
#include <stdbool.h>

enum MDFU_FIRMWARE_UPDATE_RESULT
{
    MDFU_FIRMWARE_UPDATE_PROTOCOL_PASS,
    MDFU_FIRMWARE_UPDATE_PROTOCOL_FAIL,
    MDFU_FIRMWARE_UPDATE_PROTOCOL_INVALID_LENGTH,
    MDFU_FIRMWARE_UPDATE_PROTOCOL_WRITE_ERROR,
    MDFU_FIRMWARE_UPDATE_PROTOCOL_INVALID_BUFFER
};

/**
 * @brief    Interprets and writes data to the start address known by the Firmware Update Layer. To be called by the 
 *           MDFU Command Processor Layer when a Write Chunk command is received  
 * @param [in]  buffer - Storage buffer for data to be written 
 * @param [in]  length - Number of bytes to be written
 * @return   Status indicating if the write was successful and if not, what error occurred 
 */
enum MDFU_FIRMWARE_UPDATE_RESULT MDFU_FirmwareUpdateWriteMemory(uint8_t* buffer, uint32_t length);

/**
 * @brief    Calls the MDFU Memory layer to erase the application image. To be called by the 
 *           MDFU Command Processor Layer when a Start Transfer command is received
 * @return   Status indicating if the erase was successful or not
 */
enum MDFU_FIRMWARE_UPDATE_RESULT MDFU_FirmwareUpdateEraseMemory(void);

/**
 * @brief   Verifies the application using whatever verification method was chosen for the bootloader. To be called by the 
 *          MDFU Command Processor Layer when a Get Image State command is received    
 * @return  Boolean indicating if the verification was successful
 */
bool MDFU_FirmwareUpdateVerifyImage(void);

/**
 * @brief   Sets the program counter to the application start address.   
 */
void MDFU_FirmwareUpdateStartApplication(void);

#endif 