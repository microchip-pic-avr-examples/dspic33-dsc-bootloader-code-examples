/**
 * Generated 16-bit Bootloader Source File
 * 
 * @file     application_header.S
 * 
 * @brief    Generates the application header at the required address and allocates
 *           space for fields that are generated post build.
 *
 * @skipline @version    16-bit Bootloader - 1.26.0
 *
 * @skipline             Device : dsPIC33CK256MP508
*/
/*
    (c) [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS "AS IS." 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/


/* This file puts a verification of the application header as well as the 
 * application header itself at the start of the application image that the 
 * bootloader will use to verify the integrity of the application image.
 */

#include "mdfu/mdfu_config.h"
#include "mdfu/mdfu_application_header.h"
    
    .section .crc_of_header, code, address(MDFU_CONFIG_HEADER_VERIFICATION_ADDRESS), keep
 
    /* Reserve space for header verification value (checksum/crc/signature/etc.) */
    .space 4, 0

    .section .application_header, code, address(MDFU_CONFIG_HEADER_ADDRESS_LOW), keep

    /* TLV Code Size Data
     * 
     * Type - CODE_SIZE
     * Length - 4 bytes
     * Value - Number of bytes in the application image.
     */ 
    .long MDFU_APPLICATION_HEADER_CODE_SIZE
    .long 4                       
    .long MDFU_CONFIG_PROGRAMMABLE_CODE_SIZE
    
    /* TLV Version Number Data
     * 
     * Type - VERSION_NUMBER
     * Length - 4 bytes
     * Value - 32-bit number in the format of:
     *   0x00MMmmpp
     *     MM = Major revision
     *     mm = minor revision
     *     pp = patch revision
     *
     *     e.g. 0x00020A03 = version 2.10.3
     */ 
    .long MDFU_APPLICATION_HEADER_CODE_VERSION_NUMBER
    .long 4
    .long 0x00010000
    
    /* TLV Code Integrity Data
     * 
     * Type - CODE_INTEGRITY
     * Length - Determined by the verification algorithm chosen 
     * Value - The primary integrity check value for the code that must be 
     * verified before execution is allowed.  
     */ 
    .long MDFU_APPLICATION_HEADER_CODE_INTEGRITY
    .long 4
    .long 0x00000000
    
    /* TLV End of Header
     * 
     * Type - END_OF_HEADER
     * Length - 0
     * Value - Empty. Used to indicate the end of the used header space 
     * has been reached.   
     */ 
    .long MDFU_APPLICATION_HEADER_CODE_END_OF_HEADER
    .long 0
    
    
    
    


