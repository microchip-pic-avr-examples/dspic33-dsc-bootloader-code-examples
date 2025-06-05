/*
Copyright (c) [2025] Microchip Technology Inc.  

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

#include "flash_region1.h"

#if defined(TEST_FLASH_REGION1)
#include "test_flash_region1.h"
#else

#define PR1LOCK_Write(value) (PR1LOCK = (value))
#define PR1LOCK_Read() (PR1LOCK)

#define PR1CTRL_Write(value) (PR1CTRL = (value))
#define PR1CTRL_Read() (PR1CTRL)

#endif

#include <xc.h>

#define FLASH_REGION_KEY                0xB7370000U
#define FLASH_REGION_KEY_MASK           0xFFFF0000U

#define FLASH_REGION_LOCKED_UNTIL_RESET 0x00000001UL
#define FLASH_REGION_LOCKED             0x00000000UL
#define FLASH_REGION_UNLOCKED           0x00000003UL
#define FLASH_REGION_LOCK_MASK          0x00000003UL

#define FLASH_REGION_CONTROL_PERMISSIONS_MASK       0x00000070U

static bool Region1PermissionsSet(enum FLASH_REGION_PERMISSION permissions, bool lockUntilReset){
    const uint32_t lockOption = lockUntilReset ? FLASH_REGION_LOCKED_UNTIL_RESET : FLASH_REGION_LOCKED;
    uint32_t pr1ctrlUpdate = PR1CTRL_Read();
    bool result = true;
    
    PR1LOCK_Write(FLASH_REGION_KEY | FLASH_REGION_UNLOCKED);
    
    /* Disable all permissions. */
    pr1ctrlUpdate &= ~FLASH_REGION_CONTROL_PERMISSIONS_MASK;
    
    /* Re-enable the desired permissions. The permissions parameter is the 
     * inverse of the desired register settings so invert first. */
    pr1ctrlUpdate |= ~permissions;
    
    /* Write the new value to the register */
    PR1CTRL_Write(pr1ctrlUpdate);
    
    /* Lock the region */
    PR1LOCK_Write(FLASH_REGION_KEY | lockOption);
    
    if(((PR1LOCK_Read() & FLASH_REGION_LOCK_MASK) != lockOption) ||
       ((PR1CTRL_Read() & FLASH_REGION_CONTROL_PERMISSIONS_MASK) != ~permissions)) 
    {
        result = false;
    }
    
    return result;
}

const struct FLASH_REGION flashRegion1 = {
   .permissionsSet = Region1PermissionsSet
};