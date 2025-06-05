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

#ifndef MDFU_CONFIG_H
#define	MDFU_CONFIG_H

/* Corresponds to the N(c) = MaxCommandDataLength field specified in 
 * section 3.2.1 of the MDFU specification. 
 * 
 * For this implementation, this value must be an exact multiple of the minimum
 * write size (16 bytes).
 * 
 * NOTE: The parameter below is just the decoded data payload length.  This does
 * not include bytes required for the command overhead or bus encoding requirements.
 */
#define MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH (128ul)

/* Corresponds to the N(r) = MaxResponseDataLength field specified in section 3.2.1 of the MDFU specification. 
 * 
 * NOTE: The parameter below is just the decoded data payload length.  This does
 * not include bytes required for the command overhead or bus encoding requirements.
 */
#define MDFU_CONFIG_MAX_RESPONSE_DATA_LENGTH (28ul)

// Application Header Definitions 
#define MDFU_CONFIG_MAX_HEADER_LENGTH (0x1A0UL)
#define MDFU_CONFIG_HEADER_OFFSET (0x60ul)

/*******************************************************************************
 * Keystore Partition Definitions 
 * 
 * These should match the values of the regions defined in the associated
 * linker file.
 ******************************************************************************/
#define MDFU_CONFIG_KEYSTORE_DATA_ORIGIN      (0x80A000ul)
#define MDFU_CONFIG_KEYSTORE_DATA_LENGTH      (0x001000ul)

#define MDFU_CONFIG_KEYSTORE_ORIGIN           (MDFU_CONFIG_KEYSTORE_DATA_ORIGIN)
#define MDFU_CONFIG_KEYSTORE_LENGTH           (MDFU_CONFIG_KEYSTORE_DATA_LENGTH)

/*******************************************************************************
 * Executable Partition Header Definitions 
 * 
 * These should match the values of the regions defined in the associated
 * linker file.
 ******************************************************************************/
#define MDFU_CONFIG_EXECUTABLE_SIGNATURE_ORIGIN (0x80B000ul)
#define MDFU_CONFIG_EXECUTABLE_SIGNATURE_LENGTH (MDFU_CONFIG_HEADER_OFFSET)

#define MDFU_CONFIG_EXECUTABLE_HEADER_ORIGIN    (0x80B060ul)
#define MDFU_CONFIG_EXECUTABLE_HEADER_LENGTH    (MDFU_CONFIG_MAX_HEADER_LENGTH)

#define MDFU_CONFIG_EXECUTABLE_DATA_ORIGIN      (0x80B200ul)
#define MDFU_CONFIG_EXECUTABLE_DATA_LENGTH      (0x039E00ul)

#define MDFU_CONFIG_EXECUTABLE_ORIGIN           (MDFU_CONFIG_EXECUTABLE_SIGNATURE_ORIGIN)
#define MDFU_CONFIG_EXECUTABLE_LENGTH           (MDFU_CONFIG_EXECUTABLE_SIGNATURE_LENGTH + MDFU_CONFIG_EXECUTABLE_HEADER_LENGTH + MDFU_CONFIG_EXECUTABLE_DATA_LENGTH)

/*******************************************************************************
 * Download Partition Header Definitions 
 * 
 * These should match the values of the regions defined in the associated
 * linker file.
 ******************************************************************************/
#define MDFU_CONFIG_DOWNLOAD_SIGNATURE_ORIGIN (0x845000ul)
#define MDFU_CONFIG_DOWNLOAD_SIGNATURE_LENGTH (MDFU_CONFIG_HEADER_OFFSET)

#define MDFU_CONFIG_DOWNLOAD_HEADER_ORIGIN    (0x845060ul)
#define MDFU_CONFIG_DOWNLOAD_HEADER_LENGTH    (MDFU_CONFIG_MAX_HEADER_LENGTH)

#define MDFU_CONFIG_DOWNLOAD_DATA_ORIGIN      (0x845200ul)
#define MDFU_CONFIG_DOWNLOAD_DATA_LENGTH      (0x039E00ul)

#define MDFU_CONFIG_DOWNLOAD_ORIGIN           (MDFU_CONFIG_DOWNLOAD_SIGNATURE_ORIGIN)
#define MDFU_CONFIG_DOWNLOAD_LENGTH           (MDFU_CONFIG_DOWNLOAD_SIGNATURE_LENGTH + MDFU_CONFIG_DOWNLOAD_HEADER_LENGTH + MDFU_CONFIG_DOWNLOAD_DATA_LENGTH)

#endif	/* MDFU_CONFIG_H */