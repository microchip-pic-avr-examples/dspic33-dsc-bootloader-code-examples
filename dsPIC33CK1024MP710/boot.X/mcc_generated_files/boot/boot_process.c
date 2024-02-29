
/**
  @Generated 16-bit Bootloader Source File

  @Company:
    Microchip Technology Inc.

  @File Name: 
    boot_process.c

  @Summary:
    This is the boot_process.c file generated using 16-bit Bootloader

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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <xc.h>
#include "com_adaptor.h"
#include "boot_private.h"
#include "boot_process.h"
#include "boot_config.h"
#include "boot_image.h"


#if ((BOOT_CONFIG_MAX_PACKET_SIZE - SIZE_OF_CMD_STRUCT_0) < MINIMUM_WRITE_BLOCK_SIZE )
#error "The maximum packet size is not large enough to store a full write block plus header. Make the max packet size larger."
#endif


static uint8_t commandArray[BOOT_CONFIG_MAX_PACKET_SIZE];

enum BOOT_COMMAND_RESPONSES
{
    COMMAND_SUCCESS = 0x01,
    UNSUPPORTED_COMMAND = 0xFF,
    BAD_ADDRESS = 0xFE,
    BAD_LENGTH  = 0xFD,
    VERIFY_FAIL = 0xFC
};

enum BOOT_COMMAND
{
    READ_VERSION = 0x00,
    READ_FLASH = 0x01,
    WRITE_FLASH = 0x02,
    ERASE_FLASH = 0x03,
    CALC_CHECKSUM = 0x08,
    RESET_DEVICE = 0x09,
    SELF_VERIFY = 0x0A,
    GET_MEMORY_ADDRESS_RANGE_COMMAND = 0x0B
};



/******************************************************************************/
/* Private Function Prototypes                                                */
/******************************************************************************/

static enum BOOT_COMMAND_RESULT CommandError(enum BOOT_COMMAND_RESPONSES);
static enum BOOT_COMMAND_RESULT ReadVersion(void);

static void ResetDevice(void);
static enum BOOT_COMMAND_RESULT EraseFlash(void);
static enum BOOT_COMMAND_RESULT WriteFlash(void);
static enum BOOT_COMMAND_RESULT ReadFlash(void);
static enum BOOT_COMMAND_RESULT SelfVerify(void);
static enum BOOT_COMMAND_RESULT GetMemoryAddressRange(void);
static void AIVTEnable(bool enable);

/******************************************************************************/
/* Public Functions                                                           */
/******************************************************************************/
void BOOT_Initialize() 
{
    //  When CodeGuard is enabled, the AIVT is used for the application.  The bootloader should run in 
    //  regular interrupt table mode.
    AIVTEnable(false);
}


enum BOOT_COMMAND_RESULT BOOT_ProcessCommand(void)
{    
    uint16_t bytes_ready = BOOT_COM_GetBytesReady();
    uint8_t command;
    uint16_t command_length;

    if (bytes_ready == 0u) 
    { 
        return BOOT_COMMAND_NONE; 
    }
    if (bytes_ready < sizeof(struct CMD_STRUCT_0)) 
    { 
        return BOOT_COMMAND_INCOMPLETE; 
    }
    
    command = BOOT_COM_Peek(0);

    // validate the length of the command will not exceed the buffer size
    command_length = BOOT_COM_Peek(1u) + BOOT_COM_Peek(2u)*256u + sizeof(struct CMD_STRUCT_0);
    if ( ( command_length > BOOT_CONFIG_MAX_PACKET_SIZE ) && ( command != ERASE_FLASH ) )
    {
        return CommandError(BAD_LENGTH);
    }

    switch (command)
    {
        
    case WRITE_FLASH:
        return WriteFlash();

    case READ_FLASH:
        return ReadFlash();

    case READ_VERSION:
        return ReadVersion();

    case ERASE_FLASH:
        return EraseFlash();

    case RESET_DEVICE:
        ResetDevice();
        return BOOT_COMMAND_SUCCESS;


    case SELF_VERIFY:
        return SelfVerify();

     
    case GET_MEMORY_ADDRESS_RANGE_COMMAND:
        return GetMemoryAddressRange();

    default:
        return CommandError(UNSUPPORTED_COMMAND);
    }
    
    return BOOT_COMMAND_ERROR;
}

void BOOT_StartApplication()
{
//  When CodeGuard is enabled, the AIVT is used for the application.  The application should run in 
//  alternate interrupt table mode.
    AIVTEnable(true);

    int (*user_application)(void);
    user_application = (int(*)(void))BOOT_CONFIG_APPLICATION_RESET_ADDRESS;
    user_application();       
}  

/******************************************************************************/
/* Private Functions                                                          */
/******************************************************************************/

static void AIVTEnable(bool enable)
{
#if defined(_ALTIVT)
    _ALTIVT = enable;
#elif defined(_AIVTEN)
    _AIVTEN = enable;
#else
    #error "Unknown/unsupported device type.  Implement support for switching to alternate interrupt table mode."
#endif
}

static enum BOOT_COMMAND_RESULT CommandError(enum BOOT_COMMAND_RESPONSES errorType)
{
    struct RESPONSE_TYPE_0 response;
    
    (void)BOOT_COM_Read(commandArray, sizeof(struct CMD_STRUCT_0));
    memcpy(&response, commandArray, sizeof(struct CMD_STRUCT_0));
    response.success = errorType;
    BOOT_COM_Write((uint8_t*) & response, sizeof (struct RESPONSE_TYPE_0 ) / sizeof (uint8_t));
    
    return BOOT_COMMAND_ERROR;
}

static enum BOOT_COMMAND_RESULT ReadVersion(void)
{
    struct GET_VERSION_RESPONSE response = {
        .cmd = 0,
        .dataLength = 0,
        .unlockSequence = 0,
        .address = 0,
        .version = BOOT_CONFIG_VERSION,
        .maxPacketLength = BOOT_CONFIG_MAX_PACKET_SIZE,
        .unused1 = 0,
        .deviceId = 0x3456u,
        .unused2 = 0,
        .writeSize = MINIMUM_WRITE_BLOCK_SIZE,
        .unused3 = 0,
        .userRsvdStartSddress = 0,
        .userRsvdEndSddress = 0
    };
    response.eraseSize = BOOT_EraseSizeGet();
    
    (void)BOOT_COM_Read(commandArray, sizeof(struct CMD_STRUCT_0));
    BOOT_COM_Write((uint8_t*) & response, sizeof (struct GET_VERSION_RESPONSE ) / sizeof (uint8_t));

    return BOOT_COMMAND_SUCCESS;
}


static void Reset(void){
    asm ("reset");
}

static void ResetDevice(void)
{
    struct RESPONSE_TYPE_0 response = {
        .cmd = 9,
        .dataLength = 0,
        .unlockSequence = 0,
        .address = 0,

        .success = COMMAND_SUCCESS
    };         
    
    (void)BOOT_COM_Read(commandArray, sizeof(struct CMD_STRUCT_0));

    BOOT_COM_Write((uint8_t*) & response, sizeof (struct RESPONSE_TYPE_0) / sizeof (uint8_t));
    
    Reset();
 }


static enum BOOT_COMMAND_RESULT EraseFlash(void)
{
    struct RESPONSE_TYPE_0 response;
    struct CMD_STRUCT_0 *pCommand = (struct CMD_STRUCT_0*) commandArray; 
    
    (void)BOOT_COM_Read(commandArray, sizeof(struct CMD_STRUCT_0));
            
    memcpy(&response, commandArray, sizeof(struct CMD_STRUCT_0));

    response.success = BAD_ADDRESS;
    if ( BOOT_BlockErase(pCommand->address, pCommand->dataLength, pCommand->unlockSequence) == NVM_SUCCESS)
    {
        response.success = COMMAND_SUCCESS;
    }     
    
    BOOT_COM_Write((uint8_t*) & response, sizeof (struct RESPONSE_TYPE_0) / sizeof (uint8_t));

    /* destroy the unlock key so it isn't sitting around in memory. */
    pCommand->unlockSequence = ~pCommand->unlockSequence;
    
    if(response.success == COMMAND_SUCCESS)
    {
        return BOOT_COMMAND_SUCCESS;
    }
    
    return BOOT_COMMAND_ERROR;
}

static enum BOOT_COMMAND_RESULT WriteFlash(void)
{
    struct CMD_STRUCT_0_WITH_PAYLOAD *pCommand = (struct CMD_STRUCT_0_WITH_PAYLOAD*) commandArray;
    struct RESPONSE_TYPE_0 response;
    uint16_t dataLength;
    
    dataLength = BOOT_COM_Peek(2u)<<8u;
    dataLength |= BOOT_COM_Peek(1u);
    
    if (BOOT_COM_GetBytesReady() < (sizeof(struct CMD_STRUCT_0) + dataLength))
    { 
        return BOOT_COMMAND_INCOMPLETE; 
    }  
    
    (void)BOOT_COM_Read(commandArray, sizeof(struct CMD_STRUCT_0) + dataLength);

    memcpy(&response, commandArray, sizeof(struct CMD_STRUCT_0));
    
    response.success = COMMAND_SUCCESS;


    if (dataLength <= (BOOT_CONFIG_MAX_PACKET_SIZE - sizeof(struct CMD_STRUCT_0)))
    {
        if (BOOT_BlockWrite(pCommand->address, dataLength, &pCommand->data[0], pCommand->unlockSequence) != NVM_SUCCESS)
        {
            response.success = BAD_ADDRESS;   
        } 
    }   
    else
    {
        response.success = BAD_ADDRESS;
    }
    
    BOOT_COM_Write((uint8_t*) & response, sizeof (struct RESPONSE_TYPE_0) / sizeof (uint8_t));
    
    /* destroy the unlock key so it isn't sitting around in memory. */
    pCommand->unlockSequence = ~pCommand->unlockSequence;
    
    if(response.success == COMMAND_SUCCESS)
    {
        return BOOT_COMMAND_SUCCESS;
    }
    
    return BOOT_COMMAND_ERROR;
}
static enum BOOT_COMMAND_RESULT ReadFlash(void)
{
    struct RESPONSE_TYPE_0_WITH_PAYLOAD response;
    struct CMD_STRUCT_0_WITH_PAYLOAD * const pCommand = (struct CMD_STRUCT_0_WITH_PAYLOAD*) commandArray;
    
   (void)BOOT_COM_Read(commandArray, sizeof(struct CMD_STRUCT_0) );

    memcpy(&response, commandArray, sizeof(struct CMD_STRUCT_0));

    if (pCommand->dataLength <= (BOOT_CONFIG_MAX_PACKET_SIZE - sizeof(struct CMD_STRUCT_0) - 1u ))
    {
        NVM_RETURN_STATUS readStatus = BOOT_BlockRead ((uint8_t *)&response.data[0], pCommand->dataLength, pCommand->address );
        if (readStatus == NVM_SUCCESS)
        {    
            response.success = COMMAND_SUCCESS;
        } 
        else
        {
            response.success = BAD_ADDRESS;
            response.dataLength = 0u;        
        }
            
    } 
    else
    {
        response.success = BAD_ADDRESS;
        response.dataLength = 0u;        
    }
    
    BOOT_COM_Write((uint8_t*) & response, (sizeof (struct RESPONSE_TYPE_0) / sizeof (uint8_t)) + response.dataLength  );

    if(response.success == COMMAND_SUCCESS)
    {
        return BOOT_COMMAND_SUCCESS;
    }
    
    return BOOT_COMMAND_ERROR;
}



static enum BOOT_COMMAND_RESULT SelfVerify(void)
{
    struct RESPONSE_TYPE_0_2_PAYLOAD response;
    
    (void)BOOT_COM_Read((uint8_t*)&response, sizeof(struct CMD_STRUCT_0) );

    if(BOOT_ImageVerify(DOWNLOAD_IMAGE_NUMBER) == false)
    {
        response.success = VERIFY_FAIL;
    }
    else
    {
        response.success = COMMAND_SUCCESS;
    }

    response.dataLength = 0;
    BOOT_COM_Write((uint8_t*) &response, (sizeof (struct RESPONSE_TYPE_0) / sizeof (uint8_t)) );  
    
    if(response.success == COMMAND_SUCCESS)
    {
        return BOOT_COMMAND_SUCCESS;
    }
    
    return BOOT_COMMAND_ERROR;
} 

static enum BOOT_COMMAND_RESULT GetMemoryAddressRange(void)
{
   struct GET_MEMORY_ADDRESS_RANGE_RESPONSE response = {
        .cmd = 0xB,
        .dataLength = 0x8,
        .unlockSequence = 0x0,
        .address = 0,   
        .success = 1,
        .programFlashStart = EXECUTABLE_IMAGE_FIRST_ADDRESS,
        .programFlashEnd = EXECUTABLE_IMAGE_LAST_ADDRESS
    };
    (void)BOOT_COM_Read(commandArray, sizeof(struct CMD_STRUCT_0));
    BOOT_COM_Write((uint8_t*) & response, sizeof (struct GET_MEMORY_ADDRESS_RANGE_RESPONSE ) / sizeof (uint8_t));

    return BOOT_COMMAND_SUCCESS;
}



   



