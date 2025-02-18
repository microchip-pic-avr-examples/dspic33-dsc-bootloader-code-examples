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

#include <stdbool.h>
#include <stdint.h>
#include "mdfu_boot_demo.h"
#include "mdfu_verification.h"
#include "mdfu_command_processor.h"
#include "mdfu_firmware_update.h"
#include "mdfu_transport.h"
#include "../mcc_generated_files/system/interrupt.h"
#include "../mcc_generated_files/uart/uart1.h"
#include "mdfu_partition.h"

#include "../bsp/s1.h"
#include "../bsp/led0.h"

#include "mdfu_boot_entry.h"

extern const struct PARTITION application;
extern const struct PARTITION boot;

static enum MDFU_VERIFY_CODE executionPartitionValid = MDFU_VERIFY_CODE_INVALID_PARTITION;
static enum MDFU_COMMAND_RESULT status = MDFU_COMMAND_SESSION_WAITING;

static bool UserRequestedFirmwareUpdate(void);

enum BOOT_STATE 
{
    BOOT_STATE_BOOTING,
    BOOT_STATE_FIRMWARE_UPDATE_MODE
};

static enum BOOT_STATE state = BOOT_STATE_BOOTING;

#ifndef RESET
static void Reset(void)
{
    asm ("reset");
}
#else
extern void Reset(void);
#endif

void MDFU_BootDemoInitialize(void)
{ 
    /* Lock the boot partition as executable only. This is important if not
     * using the IRT option to make sure that application code is unable to
     * write to the boot partition. */    
    if(boot.modeChange(PARTITION_MODE_EXECUTABLE & PARTITION_MODE_READ & PARTITION_MODE_LOCKED) != PARTITION_STATUS_SUCCESS)
    {
        Reset();
    }
            
    executionPartitionValid = MDFU_VERIFY_CODE_INVALID_PARTITION;
    status = MDFU_COMMAND_SESSION_WAITING;
    state = BOOT_STATE_BOOTING;
    LED0_Initialize();
    
    BUTTON_S1_Initialize();
}

void MDFU_BootDemoTasks(void)
{
    switch(state)
    {
        case BOOT_STATE_BOOTING:
            if((UserRequestedFirmwareUpdate() == true ) ||
               (MDFU_Verify(&application) != MDFU_VERIFY_CODE_SUCCESS))
            {
                /* Make the application image readable and writable. */
                if(application.modeChange(PARTITION_MODE_READ & PARTITION_MODE_WRITE) != PARTITION_STATUS_SUCCESS)
                {
                    Reset();
                }
                state = BOOT_STATE_FIRMWARE_UPDATE_MODE;
                
                LED0_On();
            }
            else
            {
                /* NOTE: Before starting the application, all interrupts used
                 * by the bootloader must be disabled. Add code here to return
                 * the peripherals/interrupts to the reset state before starting
                 * the application code. */
                #warning "All interrupt sources and peripherals should be disabled before starting the application.  Add any code required here to disable all interrupts and peripherals used in the bootloader."
                INTERRUPT_GlobalDisable();
                INTERRUPT_Deinitialize();
    
                /* Make the application image executable, readable, write protected, and locked. */
                if(application.modeChange(PARTITION_MODE_EXECUTABLE & PARTITION_MODE_READ & PARTITION_MODE_LOCKED) != PARTITION_STATUS_SUCCESS)
                {
                    Reset();
                } 
                else 
                {
                    MDFU_FirmwareUpdateStartApplication();   
                }
            }
            break;
            
        case BOOT_STATE_FIRMWARE_UPDATE_MODE:           
            status = MDFU_CommandProcess();
            
            /* If we have completed an update, reset the hardware and attempt
             * to boot the image.  
             *
             * If we have failed an update, reset and attempt to recover.
             */
            if ((status == MDFU_COMMAND_SESSION_COMPLETE) ||
                (status == MDFU_COMMAND_SESSION_FAILED))
            {
                Reset();
            }
            break;
    }
}

static bool UserRequestedFirmwareUpdate(void)
{
    bool applicationRequestedBootEntry = MDFU_BootEntryIsRequested();
    
    /* We have recorded the applications  request to enter boot mode above,
     * clear the entry key so that the next reset will return to the 
     * application regardless.
     */
    MDFU_BootEntryRequestClear();
    
    #warning "Update this function to return 'true' when you want to stay in the boot loader, and 'false' when you want to allow a release to the application code"
 
    /* NOTE: This might be a a push button status on power up, a command from a peripheral, 
     * or whatever is specific to your boot loader implementation */    

    return BUTTON_S1_IsPressed() || applicationRequestedBootEntry;
}



