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
#define MDFU_CONFIG_MAX_RESPONSE_DATA_LENGTH (28u)

// Application Header Definitions 
#define MDFU_CONFIG_HEADER_VERIFICATION_ADDRESS (0x805000u)
#define MDFU_CONFIG_HEADER_ADDRESS_LOW (0x805004u)
#define MDFU_CONFIG_MAX_HEADER_LENGTH (124ul)
#define MDFU_CONFIG_HEADER_ADDRESS_HIGH (MDFU_CONFIG_HEADER_ADDRESS_LOW + MDFU_CONFIG_MAX_HEADER_LENGTH)
#define MDFU_CONFIG_HEADER_OFFSET (4ul)

// Application Image Definitions 
#define MDFU_CONFIG_APPLICATION_ADDRESS_LOW (MDFU_CONFIG_HEADER_VERIFICATION_ADDRESS)
#define MDFU_CONFIG_APPLICATION_ADDRESS_HIGH (0x81FFFFu)
#define MDFU_CONFIG_CODE_OFFSET (MDFU_CONFIG_HEADER_OFFSET + MDFU_CONFIG_MAX_HEADER_LENGTH)
#define MDFU_CONFIG_APPLICATION_RESET_ADDRESS (MDFU_CONFIG_HEADER_ADDRESS_HIGH)

#endif	/* MDFU_CONFIG_H */

