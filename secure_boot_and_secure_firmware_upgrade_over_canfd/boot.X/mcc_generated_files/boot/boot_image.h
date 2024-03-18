

/**
  @Generated 16-bit Bootloader Source File

  @Company:
    Microchip Technology Inc.

  @File Name: 
    boot_image.h

  @Summary:
    This is the boot_image.h file generated using 16-bit Bootloader

  @Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  16-bit Bootloader - 1.25.0
        Device            :  dsPIC33CK1024MP710
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36B
        MPLAB             :  MPLAB X v5.15
*/
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

#ifndef BOOT_IMAGE_H
#define BOOT_IMAGE_H

#include <stdbool.h>
#include <stdint.h>
#include "boot_config.h"

#define DOWNLOAD_IMAGE_NUMBER 1u
#define BOOT_IMAGE_COUNT 2
#define BOOT_IMAGE_SIZE ((unsigned long)BOOT_CONFIG_DOWNLOAD_HIGH - (unsigned long)BOOT_CONFIG_DOWNLOAD_LOW + (unsigned long)2)

#define EXECUTABLE_IMAGE_FIRST_ADDRESS BOOT_CONFIG_PROGRAMMABLE_ADDRESS_LOW
#define EXECUTABLE_IMAGE_LAST_ADDRESS ((unsigned long)BOOT_CONFIG_PROGRAMMABLE_ADDRESS_LOW + (unsigned long)BOOT_IMAGE_SIZE - (unsigned long)2)

enum BOOT_IMAGE{
    BOOT_IMAGE_0 = 0,
    BOOT_IMAGE_1 = 1,
};

typedef enum NVM_RETURN_STATUS {
    NVM_SUCCESS,
    NVM_INVALID_LENGTH,
    NVM_INVALID_ADDRESS,
    NVM_WRITE_ERROR,
    NVM_READ_ERROR,
}NVM_RETURN_STATUS; 


bool IsLegalRange(uint32_t startRangeToCheck, uint32_t endRangeToCheck);
bool BOOT_ImageErase(enum BOOT_IMAGE image);
void BOOT_CopyLock(void);
void BOOT_CopyUnlock(void);
bool BOOT_ImageCopy(enum BOOT_IMAGE destinationImage, enum BOOT_IMAGE sourceImage);

bool BOOT_ImageVerify(enum BOOT_IMAGE image);
uint32_t BOOT_ImageAddressGet(enum BOOT_IMAGE image, uint32_t addressInExecutableImage);
uint16_t BOOT_EraseSizeGet();
NVM_RETURN_STATUS BOOT_BlockWrite(uint32_t nvmAddress, uint32_t lengthInBytes, uint8_t *sourceData, uint32_t key);
NVM_RETURN_STATUS BOOT_BlockRead (uint8_t *destinationData, uint32_t lengthInBytes, uint32_t nvmAddress);
NVM_RETURN_STATUS BOOT_BlockErase (uint32_t nvmAddress, uint32_t lengthInBytes, uint32_t key);

// Functions to read RAM data stored in flash in 16 bit values in each instruction location.
NVM_RETURN_STATUS BOOT_Read32Data (uint32_t *destinationData,  uint32_t nvmAddress);
NVM_RETURN_STATUS BOOT_Read16Data (uint16_t *destinationData,  uint32_t nvmAddress);
#endif