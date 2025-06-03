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

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "mdfu_boot_demo.h"
#include "mdfu_verification.h"
#include "mdfu_command_processor.h"
#include "mdfu_firmware_update.h"
#include "mdfu_transport.h"
#include "../mcc_generated_files/uart/uart1.h"
#include "../mcc_generated_files/system/interrupt.h"
#include "mdfu_partition.h"
#include "mdfu_partition_header.h"

#include "../bsp/s3.h"
#include "../bsp/led0.h"
#include "../bsp/led1.h"

#include "mdfu_boot_entry.h"
#include "mdfu_recovery_authorization.h"

#include "mdfu_partition_executable.h"
#include "mdfu_partition_download.h"
#include "mdfu_partition_boot.h"

static enum MDFU_COMMAND_RESULT status = MDFU_COMMAND_SESSION_WAITING;

static bool UserRequestedFirmwareUpdate(void);
static void LaunchExecutable(void);

enum BOOT_STATE {
    BOOT_STATE_RESET,
    BOOT_STATE_BOOTING,
    BOOT_STATE_RECOVER_FROM_DOWNLOAD,
    BOOT_STATE_INSTALL_UPGRADE,
    BOOT_STATE_FIRMWARE_UPDATE_MODE
};

static enum BOOT_STATE state = BOOT_STATE_RESET;

#ifndef RESET
static void Reset(void){
#ifdef __DEBUG    
    /* If we are in debug mode, cause a software breakpoint in the debugger */
    __builtin_software_breakpoint();
    while(1)
    {
    
    }
#else
    // Trigger software reset
    __asm__ volatile ("reset");
#endif
}
#else
extern void Reset(void);
#endif

void MDFU_BootDemoInitialize(void)
{ 
    /* Lock the boot partition as executable only. This is important if not
     * using the IRT option to make sure that executable code is unable to
     * write to the boot partition. */    
    if(boot.modeChange(MDFU_PARTITION_MODE_EXECUTABLE & MDFU_PARTITION_MODE_READ & MDFU_PARTITION_MODE_LOCKED) != MDFU_PARTITION_STATUS_SUCCESS)
    {
        Reset();
    }
            
    status = MDFU_COMMAND_SESSION_WAITING;
    state = BOOT_STATE_RESET;
    LED0_Initialize();
    
    BUTTON_S3_Initialize();
}

void MDFU_BootDemoTasks(void)
{
    switch(state)
    {
        case BOOT_STATE_RESET:
            if(UserRequestedFirmwareUpdate() == true) 
            {
                state = BOOT_STATE_FIRMWARE_UPDATE_MODE;
            }
            else
            {
                state = BOOT_STATE_BOOTING;
            }
            break;
        case BOOT_STATE_BOOTING:
            if(MDFU_Verify(&executable) != MDFU_VERIFY_CODE_SUCCESS)
            {
                state = BOOT_STATE_RECOVER_FROM_DOWNLOAD;
            }
            else if ((MDFU_BootIsInstallationAllowed() &&
                      MDFU_Verify(&download) == MDFU_VERIFY_CODE_SUCCESS))
            {
                state = BOOT_STATE_INSTALL_UPGRADE;
            }
            else
            {
                //Next state if LaunchExecutable fails BOOT_STATE_RESET
                LaunchExecutable();
                Reset();
            }
            break;
        case BOOT_STATE_RECOVER_FROM_DOWNLOAD:
            if((MDFU_RecoveryAuthorized() == true) &&
               (MDFU_Verify(&download) == MDFU_VERIFY_CODE_SUCCESS))
            {
                state = BOOT_STATE_INSTALL_UPGRADE;
            }
            else
            {
                state = BOOT_STATE_FIRMWARE_UPDATE_MODE;
            }
            break;
        case BOOT_STATE_INSTALL_UPGRADE:
            /* Make the executable partition readable and writable. */
            if(executable.modeChange(MDFU_PARTITION_MODE_READ & MDFU_PARTITION_MODE_WRITE) == MDFU_PARTITION_STATUS_SUCCESS)
            {
                executable.copy(&download); 
                /* Make the executable partition readable and locked to prevent self writes. */
                if (executable.modeChange(MDFU_PARTITION_MODE_READ & MDFU_PARTITION_MODE_LOCKED) != MDFU_PARTITION_STATUS_SUCCESS)
                {
                    Reset();
                }
            } 
            state = BOOT_STATE_BOOTING;
            break;
        case BOOT_STATE_FIRMWARE_UPDATE_MODE:    
            LED0_On();
            status = MDFU_CommandProcess();
            
            /* If we have completed an update, reset the hardware and attempt
             * to boot the image.  
             *
             * If we have failed an update, reset and attempt to recover.
             */
            if ((status == MDFU_COMMAND_SESSION_COMPLETE) ||
                (status == MDFU_COMMAND_SESSION_FAILED))
            {
                //Next state if failed BOOT_STATE_RESET
                Reset();
            }
            break;
        default:
            /* Unknown state. */
            Reset();
            break;
    }
}

static bool UserRequestedFirmwareUpdate(void)
{
    bool executableRequestedBootEntry = MDFU_BootEntryIsRequested();
    
    /* We have recorded the executable's request to enter boot mode above,
     * clear the entry key so that the next reset will return to the 
     * executable regardless.
     */
    MDFU_BootEntryRequestClear();
    
    #warning "Update this function to return 'true' when you want to stay in the boot loader, and 'false' when you want to allow a release to the executable code"
 
    /* NOTE: This might be a a push button status on power up, a command from a peripheral, 
     * or whatever is specific to your boot loader implementation */    

    return BUTTON_S3_IsPressed() || executableRequestedBootEntry;
}

static void LaunchExecutable(void)
{
    if (MDFU_VerifyPartitionIntegrity(&executable) != MDFU_VERIFY_CODE_SUCCESS) {
        Reset();
    } /* Make the executable partition executable, readable, write protected, and locked. */
    else if (executable.modeChange(MDFU_PARTITION_MODE_EXECUTABLE & MDFU_PARTITION_MODE_READ & MDFU_PARTITION_MODE_LOCKED) != MDFU_PARTITION_STATUS_SUCCESS) {
        Reset();
    } else {
        executable.run();
        //If the application fails to launch correctly then reset to the bootloader to attempt recovery.
        Reset();
    }
}

bool MDFU_BootIsInstallationAllowed(void){
    bool updatePending = false;
    
    /* Default to the lowest possible version in case either version number is
     * unable to be read.
     */
    uint32_t downloadVersion = 0;   
    uint32_t executableVersion = 0;
    
    (void)MDFU_HeaderItemBufferRead(&download, MDFU_PARTITION_HEADER_VERSION_NUMBER, sizeof(uint32_t), &downloadVersion);
    (void)MDFU_HeaderItemBufferRead(&executable, MDFU_PARTITION_HEADER_VERSION_NUMBER, sizeof(uint32_t), &executableVersion);

    if ((downloadVersion > executableVersion) && 
            (MDFU_VerifyPartitionIntegrity(&download) == MDFU_VERIFY_CODE_SUCCESS)) {
        updatePending = true;
    }
    
    return updatePending;
}
