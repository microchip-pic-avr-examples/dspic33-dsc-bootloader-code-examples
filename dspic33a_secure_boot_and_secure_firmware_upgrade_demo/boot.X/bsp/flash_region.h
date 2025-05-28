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

#ifndef FLASH_REGION_H
#define	FLASH_REGION_H

#include <stdbool.h>

/* The values of these enums are hardware specific and align with the hardware
 * implementation. These are the inverse of the actual register settings. 
 */
enum FLASH_REGION_PERMISSION {
    FLASH_REGION_PERMISSION_EXECUTION_ENABLED   = ~(1U<<4U),
    FLASH_REGION_PERMISSION_READ_ENABLED        = ~(1U<<5U),
    FLASH_REGION_PERMISSION_WRITE_ENABLED       = ~(1U<<6U),

    FLASH_REGION_PERMISSION_EXECUTION_DISABLED  = ~(0U<<4U),    
    FLASH_REGION_PERMISSION_READ_DISABLED       = ~(0U<<5U),
    FLASH_REGION_PERMISSION_WRITE_DISABLED      = ~(0U<<6U),
};

struct FLASH_REGION 
{   
    /* 
     * @brief       Sets the permissions of the flash region.  This will unlock
     *              the specified region, change the permissions, and lock the
     *              region after it is complete.  If lockUntilReset is true
     *              then it will lock the region until reset.  If false, then
     *              a normal temporary lock is used.
     * @param[in]   enum FLASH_REGION_PERMISSION permissions - The permissions
     *              that should be given to the flash region ANDED together. 
     *              e.g. - FLASH_REGION_PERMISSION_READ_ENABLED & FLASH_REGION_PERMISSION_WRITE_ENABLED
     * @param[in]   bool lockUntilReset - specifies if the region should be
     *              locked until the next device reset after permissions are set.
     * @return      bool - if the change was successful or not.  If the region is
     *              IRT, OTP, or locked, this will result in a false.
     */
    bool (* const permissionsSet)(enum FLASH_REGION_PERMISSION permissions, bool lockUntilReset);
};

#endif	/* FLASH_REGION_H */

