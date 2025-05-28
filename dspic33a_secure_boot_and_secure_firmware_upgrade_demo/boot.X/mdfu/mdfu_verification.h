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
#ifndef MDFU_VERIFICATION_H
#define MDFU_VERIFICATION_H

#include <stdint.h>
#include <stdbool.h>
#include "mdfu_partition.h"

enum MDFU_VERIFY_CODE {
    MDFU_VERIFY_CODE_SUCCESS = 0,
    MDFU_VERIFY_CODE_FAILURE_UNKNOWN = -1,
    MDFU_VERIFY_CODE_INVALID_PARTITION = -2,
    MDFU_VERIFY_CODE_INVALID_INTEGRITY_CHECK = -3,
    MDFU_VERIFY_CODE_INVALID_HEADER = -4,
    MDFU_VERIFY_CODE_INVALID_SIGNATURE = -5
};

/**
 * @brief   Verifies the authenticity of the partition header.  NOTE - the 
 *          contents of the header are not checked, only the headers authenticity.  
 * @param   struct PARTITION* partition - the partition to authenticate header of.
 * @return  enum MDFU_VERIFY_CODE - code of result. MDFU_VERIFY_CODE_SUCCESS if 
 *          header is authenticated.  
 */
enum MDFU_VERIFY_CODE MDFU_VerifyHeaderAuthenticity(struct MDFU_PARTITION const * const partition);

/**
 * @brief   Verifies the integrity of the partition data.  NOTE - must verify the 
 *          authenticity of the partition header before calling this function.
 * @param   struct PARTITION* partition - the partition to integrity check.
 * @return  enum MDFU_VERIFY_CODE - code of result. MDFU_VERIFY_CODE_SUCCESS if 
 *          partition integrity check passes.  
 */
enum MDFU_VERIFY_CODE MDFU_VerifyPartitionIntegrity(struct MDFU_PARTITION const * const partition);

/**
 * @brief    Verifies the authenticity of the partition header and if authentic, 
 *           then verifies the integrity of the partition data.
 * @param    struct PARTITION* partition - the partition to integrity check.
 * @return   enum MDFU_VERIFY_CODE - code of result. MDFU_VERIFY_CODE_SUCCESS if 
 *           both checks are successful.
 */
enum MDFU_VERIFY_CODE MDFU_Verify(struct MDFU_PARTITION const * const partition);

#endif