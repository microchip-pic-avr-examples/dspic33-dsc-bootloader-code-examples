/**
  @Generated 16-bit Bootloader Source File

  @Company:
    Microchip Technology Inc.

  @File Name: 
    boot_demo.c

  @Summary:
    This is the boot_demo.c file generated using 16-bit Bootloader

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


#include <stdbool.h>
#include <stdint.h>
#include "boot_config.h"
#include "boot_application_header.h"
#include "boot_image.h"
#include "boot_process.h"

#define DOWNLOADED_IMAGE    1u
#define EXECUTION_IMAGE     0u

static bool inBootloadMode = false;
static bool executionImageRequiresValidation = true;
static bool executionImageValid = false;

static bool EnterBootloadMode(void);

void BOOT_DEMO_Initialize(void)
{    
    
}

/*******************************************************************************
 *  The check to see if you want to update or not is somewhat complex.  Below 
 *  is a list of the conditions when an update is required:
 *
 *  #1) The download image is valid and the execution image is invalid.  
 * 
 *  #2) Both images are valid, both images have version numbers, and the 
 *      download image version is newer than the execution image version.
 *
 *  #3) Both images are valid, if the download image has a version and the
 *      execution image doesn't have a version number (legacy image)
 *
 *  In all other situations, we shouldn't do an update.  Here are some examples,
 *  but might not be a complete list:
 *  
 *  #1) Download image is invalid
 *
 *  #2) Both images are valid, but the download image doesn't have a  version 
 *      number.  You can't confirm the download image is newer, thus we can't 
 *      update.
 *
 *  #3) Both images are valid, both have version numbers, and the execution
 *      image version number is newer or the same as the download image version
 *      number.
 ******************************************************************************/
static bool IsUpdateRequired(void)
{
    bool downloadVersionPresent;
    bool downloadImageValid;
    uint32_t downloadVersion;

    bool executionVersionPresent;
    uint32_t executionVersion;
    
    bool updateRequired = false;
    
    executionVersionPresent = BOOT_VersionNumberGet(EXECUTION_IMAGE, &executionVersion);
    downloadVersionPresent = BOOT_VersionNumberGet(DOWNLOADED_IMAGE, &downloadVersion);
    
    downloadImageValid = BOOT_ImageVerify(DOWNLOADED_IMAGE);
    executionImageValid = BOOT_ImageVerify(EXECUTION_IMAGE);
    
    /* We don't need to validate the execution image unless we update it. */
    executionImageRequiresValidation = false;
    
    if(downloadImageValid == true)
    {
        if(executionImageValid == false)
        {
            /* the download image is valid and 
             * the execution image is not valid = update.  In this case we don't
             * care about the version numbers since the download image is the
             * only valid image available. */
            updateRequired = true;
        }
        else
        {
            if(downloadVersionPresent == true )
            {
                if(executionVersionPresent == true)
                {
                    if(downloadVersion > executionVersion)
                    {
                        /* both images are valid, both have version numbers, and
                         * the download has a newer version number = update */
                        updateRequired = true;
                    }
                }
                else
                {
                    /* both images are valid, the download has a version number,
                     *  the executable does not (legacy - thus older) = update*/
                    updateRequired = true;
                }
            }
        }
    }
    
    return updateRequired;
}

static void UpdateFromDownload(void)
{
    BOOT_CopyUnlock();
    if( IsUpdateRequired() == true )
    {
        /* We've updated the application image.  It needs to be re-verified
         * after being copied. */
        executionImageRequiresValidation = true;

        BOOT_ImageCopy(EXECUTION_IMAGE, DOWNLOADED_IMAGE);
    }
    BOOT_CopyLock();
}

void BOOT_DEMO_Tasks(void)
{
    if(inBootloadMode == false)
    {
        if( EnterBootloadMode() == true )
        {
            inBootloadMode = true;
        }
        else
        {
            UpdateFromDownload();

            if( executionImageRequiresValidation == true )
            {
                executionImageValid = BOOT_ImageVerify(EXECUTION_IMAGE);
            }

            if(executionImageValid == false)
            {
                inBootloadMode = true;
            }

            if(inBootloadMode == false)
            {
                /* NOTE: Before starting the application, all interrupts used
                 * by the bootloader must be disabled. Add code here to return
                 * the peripherals/interrupts to the reset state before starting
                 * the application code. */
				 #warning "All interrupt sources and peripherals should be disabled before starting the application.  Add any code required here to disable all interrupts and peripherals used in the bootloader."
				MCCP9_Timer_Stop();
                BOOT_StartApplication();
            }
        }
    }
    
    (void)BOOT_ProcessCommand();
}

static bool EnterBootloadMode(void)
{
    #warning "Update this function to return 'true' when you want to stay in the boot loader, and 'false' when you want to allow a release to the application code"
 
    /* NOTE: This might be a a push button status on power up, a command from a peripheral, 
     * or whatever is specific to your boot loader implementation */    

    return false;
}
