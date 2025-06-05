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

#ifndef MDFU_COMMANDS_H
#define MDFU_COMMANDS_H

// MDFU Section 3-2 Command Response Pairs
#define MDFU_SEQUENCE_FIELD_SIZE                (0x1U)
#define MDFU_CODE_FIELD_SIZE                    (0x1U)
#define MDFU_COMMAND_RESPONSE_OVERHEAD          (MDFU_SEQUENCE_FIELD_SIZE + MDFU_CODE_FIELD_SIZE)
#define MDFU_COMMAND_SEQUENCE_OFFSET            (0x0U)
#define MDFU_COMMAND_CODE_OFFSET                (0x1U)
#define MDFU_COMMAND_DATA_OFFSET                (0x2U)

// MDFU Section 3-2-1 Response Status Codes
#define MDFU_RESPONSE_SEQUENCE_OFFSET           (0x0U)
#define MDFU_RESPONSE_STATUS_OFFSET             (0x1U)
#define MDFU_RESPONSE_DATA_OFFSET               (0x2U)
#define MDFU_RESPONSE_SUCCESS                   (0x1U)
#define MDFU_RESPONSE_COMMAND_NOT_SUPPORTED     (0x2U) // MDFU Section 3-2-2-1
#define MDFU_RESPONSE_COMMAND_NOT_EXECUTED      (0x4U) // MDFU Section 3-2-2-2
#define MDFU_RESPONSE_ABORT_FILE_TRANSFER       (0x5U) // MDFU Section 3-2-2-3

// MDFU Section 3-2-1-1 Get Client Info
#define MDFU_COMMAND_GET_CLIENT_INFO    (0x1U)

// MDFU Section 3-2-1-1-1 Protocol Version Parameter
#define MDFU_PROTOCOL_VERSION           (0x1U)
#define MDFU_PROTOCOL_VERSION_LENGTH    (0x3U)

//user defined may move to mdfu_config.h
#define MDFU_MAJOR_VERSION              (0x00U)
#define MDFU_MINOR_VERSION              (0x01U)
#define MDFU_PATCH_VERSION              (0x00U)

// MDFU Section 3-2-1-1-2 Client Buffer Info Parameter
#define MDFU_CLIENT_BUFFER_INFO         (0x2U)
#define MDFU_CLIENT_BUFFER_INFO_LENGTH  (0x3U)

//user defined may move to mdfu_config.h
#define MDFU_NUM_CMD_BUFFERS            (0x01U)

// MDFU Section 3-2-1-1-3 Client Command Timeout Parameter
#define MDFU_CLIENT_TIMEOUT             (0x3U)
#define MDFU_CLIENT_TIMEOUT_LENGTH      (0x3U)

//user defined may move to mdfu_config.h
#define MDFU_DEFAULT_CMD_TIMEOUT_CODE   (0x00U)
#define MDFU_DEFAULT_COMMAND_TIMEOUT    (0x0064UL)

// MDFU Section 3-2-1 Command Response Pair Details
#define MDFU_COMMAND_RESPONSE_SUCCESS   (0x01U)
// MDFU Section 3-2-1-2 Start Transfer Command
#define MDFU_COMMAND_START_TRANSFER     (0x02U)

// MDFU Section 3-2-1-3 Write Chunk Command
#define MDFU_COMMAND_WRITE_CHUNK        (0x03U)

// MDFU Section 3-2-1-4 Get Image State Command
#define MDFU_COMMAND_GET_IMAGE_STATE    (0x04U)

#define MDFU_RESPONSE_IMAGE_VALID       (0x01U)
#define MDFU_RESPONSE_IMAGE_INVALID     (0x02U)
#define MDFU_RESPONSE_IMAGE_DOWNREV     (0x03U)

// MDFU Section 3-2-1-5 End Transfer Command
#define MDFU_COMMAND_END_TRANSFER       (0x05U)

// MDFU Section 3-2-2-1 COMMAND_NOT_SUPPORTED Response
#define MDFU_COMMAND_RESPONSE_NOT_SUPPORTED         (0x02U)

// MDFU Section 3-2-2-2 COMMAND_NOT_EXECUTED Response
#define MDFU_COMMAND_RESPONSE_COMMAND_NOT_EXECUTED  (0x04U)
enum MDFU_COMMAND_NOT_EXECUTED_CAUSE
{
    MDFU_TRANSPORT_INTEGRITY_CHECK_ERROR    = (0x00U),
    MDFU_COMMAND_TOO_LONG                   = (0x01U),
    MDFU_COMMAND_TOO_SHORT                  = (0x02U),
    MDFU_SEQUENCE_NUMBER_INVALID            = (0x03U),
};

// MDFU Section 3-2-2-3 ABORT_FILE_TRANSFER Response
#define MDFU_COMMAND_RESPONSE_ABORT_FILE_TRANSFER   (0x05U)

// MDFU Section 3-8 Sequence Numbers
#define MDFU_SEQUENCE_SNYC_MASK         (0x80U)
#define MDFU_SEQUENCE_C_SEQUENCE_MASK   (0x1FU)
#define MDFU_SEQUENCE_NACKRESEND_MASK   (0x40U)
#define MDFU_SEQUENCE_R_SEQUENCE_MASK   (0x1FU)

#endif	/* MDFU_COMMANDS_H */

