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
#include <string.h>
#include <stdbool.h>
#include "mdfu_config.h"
#include "mdfu_commands.h"
#include "mdfu_firmware_update.h"
#include "mdfu_command_processor.h"
#include "mdfu_transport.h"

#if ((MDFU_CONFIG_APPLICATION_RESET_ADDRESS % 0x40) != 0)
#error "The application image start address/the IVTBASE address must be "  "0x40" " aligned. \
Refer to the Register Summary and the Interrupt Vector Base Address Register sections in the datasheet for details. https://www.microchip.com/en-us/product/dsPIC33AK128MC106#Documentation"
#endif

#if ((MDFU_CONFIG_HEADER_VERIFICATION_ADDRESS + MDFU_CONFIG_HEADER_OFFSET) != MDFU_CONFIG_HEADER_ADDRESS_LOW )
#error "The header must immediately follow the header verification value. Please adjust the header address or the header verification address."
#endif

#if (MDFU_CONFIG_APPLICATION_RESET_ADDRESS != MDFU_CONFIG_HEADER_ADDRESS_HIGH)
#error "The application image must immediately follow the header padding. Please adjust the application image location or the header location."
#endif

#define MDFU_CONFIG_CLIENT_INFO_SIZE (0x0Fu)

enum MDFU_SEQUENCE_STATE
{
    MDFU_SEQUENCE_STATUS_CONTINUE_PROCESSING,
    MDFU_SEQUENCE_STATUS_RESEND_LAST_RESPONSE,
    MDFU_SEQUENCE_STATUS_RESEND_REQUEST_NEXTSEQNUM 
};// MDFU Section 3-8 Client Sequence Number Processing

typedef struct
{
    unsigned int c_sequence;
    unsigned int code;
    uint8_t* dataPayload;
    uint16_t dataLength;
} command_parser;

typedef struct
{
    unsigned int r_sequence;
    bool    resend;
    unsigned int status;
    uint8_t dataPayload[MDFU_CONFIG_MAX_RESPONSE_DATA_LENGTH];
    uint16_t dataLength;
} response_parser;

static bool getSyncField(uint8_t c_sequence);
static uint8_t getSequenceWithResend (bool resendRequest, uint8_t r_sequence);
static uint8_t getCSequenceNumber (uint8_t c_sequence);
static enum MDFU_SEQUENCE_STATE checkSequenceNumber (uint8_t c_sequence);
static enum MDFU_TRANSPORT_RESULT receiveCommand(command_parser* receivedCommand);
static enum MDFU_COMMAND_RESULT executeCommand (command_parser* command, response_parser* response);
static void sendResponse(response_parser const *response);
static uint8_t mdfuGetClientInfo(uint8_t* clientInfo);
static void mdfuGetErrorResponse(response_parser* response);

static uint8_t NextSeqNum = 0;

/**
 * @brief    Processes any incoming commands from the MDFU host and returns the 
 *           status of the current transfer to the user application code.  
 * @return   Status indicating the current state of the host transfer session  
 */
enum MDFU_COMMAND_RESULT MDFU_CommandProcess(void)
{
    
    static command_parser currentCommand;
    static enum MDFU_COMMAND_RESULT sessionState = MDFU_COMMAND_SESSION_WAITING;
    static response_parser lastResponse;
    static response_parser errorResponse;
    //receive command
    switch(receiveCommand(&currentCommand))
    {
        case MDFU_TRANSPORT_COMPLETE:
            switch(checkSequenceNumber(currentCommand.c_sequence))
            {
                case MDFU_SEQUENCE_STATUS_CONTINUE_PROCESSING:
                    sessionState = executeCommand(&currentCommand, &lastResponse);
                    sendResponse(&lastResponse);
                    break; 
                case MDFU_SEQUENCE_STATUS_RESEND_LAST_RESPONSE:
                    sendResponse(&lastResponse);
                    break;
                case MDFU_SEQUENCE_STATUS_RESEND_REQUEST_NEXTSEQNUM: 
                    //This is for sequence out of order we send an error response 
                    //requesting the next expected sequence but not deleting our
                    //last valid response
                default:
                    //send special resend request with expected sequence
                    mdfuGetErrorResponse(&errorResponse);
                    sendResponse(&errorResponse);
                    break;
            }
            break;
        case MDFU_TRANSPORT_INVALID_ARG:
        case MDFU_TRANSPORT_FAIL:
            mdfuGetErrorResponse(&errorResponse);
            sendResponse(&errorResponse);
            break;
        case MDFU_TRANSPORT_BUSY:
            //Fall through to default case
        default:
            //do nothing
            break;
    }
    
    if(MDFU_TransportTransmitStatusGet() == MDFU_TRANSPORT_BUSY)
    {
        MDFU_TransportTasks();
    }
    
    return sessionState;
}

//Check for sync bit and either sync, repeat last response or increment LastSeqNum
static bool getSyncField (uint8_t c_sequence)
{
    return c_sequence & MDFU_SEQUENCE_SNYC_MASK;
}

//Mask the resend request bit into r_sequence, NACK/Resend = 1, ACK  = 0
static uint8_t getSequenceWithResend (bool resendRequest, uint8_t r_sequence)
{
    uint8_t r_sequence_val = r_sequence;
    r_sequence_val &= MDFU_SEQUENCE_R_SEQUENCE_MASK;
    if(resendRequest)
    {
        r_sequence_val |= MDFU_SEQUENCE_NACKRESEND_MASK;
    }
    
    return r_sequence_val;
}

//Get the C_SEQUENCE number without sync
static uint8_t getCSequenceNumber (uint8_t c_sequence)
{
    return c_sequence & MDFU_SEQUENCE_C_SEQUENCE_MASK;
}

static enum MDFU_SEQUENCE_STATE checkSequenceNumber (uint8_t c_sequence)
{
    static uint8_t LastSeqNum = 0;
    enum MDFU_SEQUENCE_STATE status;
    uint8_t sequenceNumber = getCSequenceNumber(c_sequence);
    if(getSyncField(c_sequence) || (sequenceNumber == NextSeqNum))
    {
        LastSeqNum = sequenceNumber;
        NextSeqNum = (sequenceNumber + 1u) % 32u;
        
        status = MDFU_SEQUENCE_STATUS_CONTINUE_PROCESSING;
    }
    else if(sequenceNumber == LastSeqNum)
    {
        status = MDFU_SEQUENCE_STATUS_RESEND_LAST_RESPONSE;
    }
    else
    {
        status = MDFU_SEQUENCE_STATUS_RESEND_REQUEST_NEXTSEQNUM;
    }
    
    return status;
}

static enum MDFU_TRANSPORT_RESULT receiveCommand (command_parser* receivedCommand)
{
    /* Needs to be static because this buffer will need to be sent in a non-blocking way 
     * so must remain after function returns.  It is in this function instead of 
     * global because MISRA identifies that this buffer is only used in one function
     * and says that its scope should be limited. */
    static uint8_t incomingFrame[MDFU_COMMAND_RESPONSE_OVERHEAD + MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH];
    
    size_t incomingFrameLength;
        
    enum MDFU_TRANSPORT_RESULT transportResult = MDFU_TRANSPORT_BUSY;
    
    transportResult = MDFU_TransportReceiveFrame(incomingFrame, sizeof(incomingFrame), &incomingFrameLength);
    if(transportResult == MDFU_TRANSPORT_COMPLETE)
    {
        receivedCommand->c_sequence = incomingFrame[MDFU_COMMAND_SEQUENCE_OFFSET];
        receivedCommand->code = incomingFrame[MDFU_COMMAND_CODE_OFFSET];
        receivedCommand->dataLength = incomingFrameLength - MDFU_COMMAND_RESPONSE_OVERHEAD;
        receivedCommand->dataPayload = &incomingFrame[MDFU_COMMAND_DATA_OFFSET];
    } 
    return transportResult;
}

static enum MDFU_COMMAND_RESULT executeCommand (command_parser* command, response_parser* response)
{
    enum MDFU_COMMAND_RESULT result = MDFU_COMMAND_SESSION_WAITING;
    
    response->dataLength = 0x0;
    response->resend = false;
    response->r_sequence = getCSequenceNumber(command->c_sequence);
    response->status = MDFU_RESPONSE_SUCCESS;
    
    //switch on command codes
    switch(command->code)
    {
        case MDFU_COMMAND_GET_CLIENT_INFO:
            response->dataLength = mdfuGetClientInfo(response->dataPayload);
            result = MDFU_COMMAND_SESSION_IN_PROGRESS;
            break;
        case MDFU_COMMAND_START_TRANSFER:
            //At this point always assume transfer is ok
            if(MDFU_FirmwareUpdateEraseMemory() == MDFU_FIRMWARE_UPDATE_PROTOCOL_PASS )
            {
                result = MDFU_COMMAND_SESSION_IN_PROGRESS;
            }             
            else
            {
                response->status = MDFU_RESPONSE_ABORT_FILE_TRANSFER;
                result = MDFU_COMMAND_SESSION_FAILED;
            } 
            break;
        case MDFU_COMMAND_WRITE_CHUNK:
            if(MDFU_FirmwareUpdateWriteMemory(command->dataPayload, command->dataLength) == MDFU_FIRMWARE_UPDATE_PROTOCOL_PASS)
            {
                result = MDFU_COMMAND_SESSION_IN_PROGRESS;
            } 
            else
            {
                response->status = MDFU_RESPONSE_ABORT_FILE_TRANSFER;
                result = MDFU_COMMAND_SESSION_FAILED;
            }   
            break;
        case MDFU_COMMAND_GET_IMAGE_STATE:
            if(MDFU_FirmwareUpdateVerifyImage())
            {
                response->dataPayload[0] = MDFU_RESPONSE_IMAGE_VALID;
            }
            else
            {
                response->dataPayload[0] = MDFU_RESPONSE_IMAGE_INVALID;
            }
            response->dataLength = 0x1;
            result = MDFU_COMMAND_SESSION_IN_PROGRESS;
            break;
        case MDFU_COMMAND_END_TRANSFER:
            //At this point assume always ok to end transfer
            result = MDFU_COMMAND_SESSION_COMPLETE;
            break;
        default:
            // for all else assume the command isn't supported
            response->status = MDFU_COMMAND_RESPONSE_NOT_SUPPORTED;
            result = MDFU_COMMAND_SESSION_IN_PROGRESS;
            break;
    }
    return result;
}

static void sendResponse(response_parser const *response)
{
    /* Needs to be static because this buffer will need to be sent in a non-blocking way 
     * so must remain after function returns.  It is in this function instead of 
     * global because MISRA identifies that this buffer is only used in one function
     * and says that its scope should be limited. */
    static uint8_t responseFrame[MDFU_CONFIG_MAX_RESPONSE_DATA_LENGTH + MDFU_COMMAND_RESPONSE_OVERHEAD];

    responseFrame[MDFU_RESPONSE_SEQUENCE_OFFSET] = getSequenceWithResend(response->resend, response->r_sequence);
    responseFrame[MDFU_RESPONSE_STATUS_OFFSET] = response->status;
    (void)memcpy(&responseFrame[MDFU_RESPONSE_DATA_OFFSET], &response->dataPayload, response->dataLength);
    
    (void)MDFU_TransportTransmitFrame(responseFrame, response->dataLength + MDFU_COMMAND_RESPONSE_OVERHEAD);
}

//will place client info in buffer you point to little endian
static uint8_t mdfuGetClientInfo(uint8_t* clientInfo)
{   
    clientInfo[0] = MDFU_PROTOCOL_VERSION;
    clientInfo[1] = MDFU_PROTOCOL_VERSION_LENGTH;
    clientInfo[2] = MDFU_MAJOR_VERSION;
    clientInfo[3] = MDFU_MINOR_VERSION;
    clientInfo[4] = MDFU_PATCH_VERSION;
    clientInfo[5] = MDFU_CLIENT_BUFFER_INFO;
    clientInfo[6] = MDFU_CLIENT_BUFFER_INFO_LENGTH;
    clientInfo[7] = (0x00FFU & MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH);
    clientInfo[8] = (MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH >> 8U);
    clientInfo[9] = MDFU_NUM_CMD_BUFFERS;
    clientInfo[10] = MDFU_CLIENT_TIMEOUT;
    clientInfo[11] = MDFU_CLIENT_TIMEOUT_LENGTH;
    clientInfo[12] = MDFU_DEFAULT_CMD_TIMEOUT_CODE;
    clientInfo[13] = (0x00FFU & MDFU_DEFAULT_COMMAND_TIMEOUT);
    clientInfo[14] = (MDFU_DEFAULT_COMMAND_TIMEOUT >> 8U);
    
    //return the size of data written
    return MDFU_CONFIG_CLIENT_INFO_SIZE;
}

static void mdfuGetErrorResponse(response_parser* response)
{
    response->dataLength = 0x0;
    response->status = MDFU_RESPONSE_COMMAND_NOT_EXECUTED;
    response->resend = true;
    response->r_sequence = NextSeqNum;
}
