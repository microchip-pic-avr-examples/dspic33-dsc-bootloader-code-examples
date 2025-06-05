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
#include <stddef.h>
#include "mdfu_transport.h"
#include "../mcc_generated_files/uart/uart1.h"
#include "mdfu_config.h"
#include <string.h>
#include "mdfu_commands.h"

#define START_OF_FRAME_CODE     (0x56U)
#define END_OF_FRAME_CODE       (0x9EU)
#define ESCAPE_CODE             (0xCCU)
#define FRAME_CHECK_SIZE        (2U)

/* The minimum/maximum frame size may vary based on the physical transport layer.
 * The MDFU protocol itself always has the following data:
 * 2 bytes command overhead for the command/sequence number
 * 0-N(c) bytes of payload data.
 * 
 * For the UART transport, a 16-bit checksum is added to the transport frame thus
 * increasing the min/max frame size at the transport layer.
 * 
 * In this implementation the receive buffer is receiving decoded data and thus
 * does not include space for the start/end of frame codes and removes all
 * escape sequence data.  
 */
#define MINIMUM_DECODED_FRAME_SIZE      (MDFU_COMMAND_RESPONSE_OVERHEAD + FRAME_CHECK_SIZE)
#define MAXIMUM_DECODED_FRAME_SIZE      (MDFU_COMMAND_RESPONSE_OVERHEAD + MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH + FRAME_CHECK_SIZE)

/* cppcheck-suppress misra-c2012-8.9
 * 
 *  (Rule 8.9) ADVISORY: An object should be defined at block scope if its
 *  identifier only appears in a single function.
 * 
 *  Reasoning: This variable creates a global buffer for receiving transmission
 *  data.  It is set to the .bufferPtr of the commandReceiver which is used
 *  elsewhere and thus can't be function scoped without leaving the 
 *  commandReceiver uninitialized.
 */
static uint8_t receiveBuffer[MAXIMUM_DECODED_FRAME_SIZE];
static bool isReceiveWindowOpen = false;
static bool isEscapedByte = false;
static enum MDFU_TRANSPORT_RESULT transmitResult = MDFU_TRANSPORT_COMPLETE;

typedef struct
{
    uint16_t receiveIndex;
    uint8_t * bufferPtr;
    enum MDFU_TRANSPORT_RESULT status;
} PROCESS_FRAME;

static PROCESS_FRAME commandReceiver = 
{
    .receiveIndex = 0,
    .bufferPtr = receiveBuffer,
    .status = MDFU_TRANSPORT_BUSY
};

/******************************************************************************/
/* Private Function Prototypes                                                */
/******************************************************************************/
static uint16_t CalculateIntegrityCheck(uint8_t * data, size_t bufferLength);
static void ClearReceiveBuffer(void);
static enum MDFU_TRANSPORT_RESULT Read(uint8_t* data, uint16_t length);
static enum MDFU_TRANSPORT_RESULT Write(uint8_t* data, uint16_t length);
static void ReceiveByte(void);
static bool UartHasAnError(void);
static void ReceiveCompleteFrame(uint8_t* frameBuffer, size_t* frameLength);
static void HandleUartError(void);
static void HandleStartOfFrame(void);
static void HandleEndOfFrame(void);
static void HandleEscapeCode(void);
static void HandleByteReceive(uint8_t nextByte);
static bool UartArgumentsValid(uint8_t const *data, uint16_t length);
static enum MDFU_TRANSPORT_RESULT WriteData(uint8_t* data, uint16_t length);
static enum MDFU_TRANSPORT_RESULT ReadData(uint8_t* data, uint16_t length);
static uint8_t DetermineNextByte(uint8_t const *transmitBuffer, uint16_t sentByteCount, uint16_t length, uint16_t integrityCheck);
static bool IsReservedCode(uint8_t nextByte);
static enum MDFU_TRANSPORT_RESULT TransmitNextByte(uint8_t nextByte);
static bool CommandReceiverHasAnError(void);

/******************************************************************************/
/* Public Functions                                                           */
/******************************************************************************/
void MDFU_TransportInitialize(void)
{
    transmitResult = MDFU_TRANSPORT_COMPLETE;
}

void MDFU_TransportTasks(void)
{
    //This implementation has a blocking transmit layer.  
}

enum MDFU_TRANSPORT_RESULT MDFU_TransportReceiveFrame(uint8_t* frameBuffer, size_t frameBufferLength, size_t* frameLength)
{
    enum MDFU_TRANSPORT_RESULT status = MDFU_TRANSPORT_BUSY;
    
    *frameLength = 0;
    
    if (( frameBuffer == NULL) ||
        ( frameBufferLength < (MDFU_COMMAND_RESPONSE_OVERHEAD + MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH) ))
    {
        status = MDFU_TRANSPORT_INVALID_ARG;
    }
    else
    {
        bool endLoop = false;
        while ((UART1_IsRxReady() || UartHasAnError()) && !endLoop) 
        {          
            ReceiveByte();

            status = commandReceiver.status;
            if (CommandReceiverHasAnError())
            {
                ClearReceiveBuffer();
                endLoop = true;
            }
            else if (status == MDFU_TRANSPORT_COMPLETE)
            {
                ReceiveCompleteFrame(frameBuffer, frameLength);
                endLoop = true;
            }
            else
            {
            }
        }
    }
    return status;
}

enum MDFU_TRANSPORT_RESULT MDFU_TransportTransmitFrame(uint8_t* transmitBuffer, uint16_t length)
{   
    transmitResult = MDFU_TRANSPORT_INVALID_ARG;
    
    if(UartArgumentsValid(transmitBuffer, length)) 
    {
        uint16_t integrityCheck = CalculateIntegrityCheck(transmitBuffer, length);
        transmitResult = Write(&(uint8_t){START_OF_FRAME_CODE}, 1U);
        if (transmitResult == MDFU_TRANSPORT_COMPLETE)
        {
            uint16_t sentByteCount = 0x00U;

            while (sentByteCount < (length + FRAME_CHECK_SIZE))
            {
                uint8_t nextByte = DetermineNextByte(transmitBuffer, sentByteCount, length, integrityCheck);
                if (TransmitNextByte(nextByte) == MDFU_TRANSPORT_COMPLETE )
                {
                    sentByteCount++;
                }
            }
            transmitResult = Write(&(uint8_t){END_OF_FRAME_CODE}, 1U);
        }
    }

    return transmitResult;
}

enum MDFU_TRANSPORT_RESULT MDFU_TransportTransmitStatusGet(void)
{
    return transmitResult;
}

/******************************************************************************/
/* Private Functions                                                          */
/******************************************************************************/

/**
 * @brief       Calls the UART driver to see if its in an error state. See note 
 *              below regarding the TX collision error.   
 * @param [in]  None.
 * @return      Boolean indicating whether the UART is reporting an actionable error.  
 */
static bool UartHasAnError(void)
{
    /* On some dsPIC33A devices, in full duplex mode, the TX collision bit
       gets set.  Ignore the collision error for purposes of error handling. */
    return (UART1_ErrorGet() & ~(uint16_t)UART_ERROR_TX_COLLISION_MASK) != 0u;
}

/**
 * @brief       Transfers the received data in the command receiver to the 
 *              buffer passed in by the caller and resets the command 
 *              receiver in preparation for the next frame.    
 * @param [in]  frameBuffer - Array in which to place the currently pending 
 *              received data.
 * @param [in]  frameLength - Length of the frame being received.      
 * @return      None.   
 */
static void ReceiveCompleteFrame(uint8_t* frameBuffer, size_t* frameLength) 
{
    for (uint16_t byteIndex = 0; byteIndex < (commandReceiver.receiveIndex - 2u); byteIndex++) 
    {
        frameBuffer[byteIndex] = commandReceiver.bufferPtr[byteIndex];
    }
    *frameLength = commandReceiver.receiveIndex - 2u;
    ClearReceiveBuffer();
}

/**
 * @brief       Reads in the next available byte from the UART Rx buffer and 
 *              determines what actions are required based on the byte received.   
 * @param [in]  None.      
 * @return      None.   
 */
static void ReceiveByte(void)
{
    uint8_t nextByte = 0U;
    enum MDFU_TRANSPORT_RESULT status = Read(&nextByte, 1U);

    if(status == MDFU_TRANSPORT_COMPLETE)
    {
        if (nextByte == START_OF_FRAME_CODE)
        {                        
           HandleStartOfFrame(); 
        }
        else if (isReceiveWindowOpen)
        {
            if (nextByte == END_OF_FRAME_CODE)
            {                            
                HandleEndOfFrame();
            }
            else if (nextByte == ESCAPE_CODE)
            {
                HandleEscapeCode();
            }
            else
            {                       
                if (isEscapedByte)
                {
                    nextByte = ~nextByte;
                    isEscapedByte = false;
                }
                HandleByteReceive(nextByte);  
            }
        }
        else 
        {
        }
    }
    else
    {
        commandReceiver.status = MDFU_TRANSPORT_FAIL;
    }
}

/**
 * @brief       Calculates the integrity check of completed incoming and 
 *              outgoing frames to ensure no data corruption occurred.
 * @param [in]  data - Data over which the checksum will be calculated.
 * @param [in]  bufferLength - Length of data provided.      
 * @return      Calculated checksum value of the data provided.    
 */
static uint16_t CalculateIntegrityCheck(uint8_t * data, size_t bufferLength)
{
    uint16_t checksum = 0;

    for(size_t bytesSummed = 0; bytesSummed<bufferLength; bytesSummed++)
    {
        size_t offset = (bytesSummed % 2U) * 8U;
        checksum += data[bytesSummed] << offset;
    }
    
    return ~checksum;
}

/**
 * @brief       Sets the command receiver back to its initial state. 
 * @param [in]  None.     
 * @return      None.    
 */
static void ClearReceiveBuffer(void)
{
    commandReceiver.receiveIndex = 0U;
    commandReceiver.status = MDFU_TRANSPORT_BUSY;
    isEscapedByte = false;
}

/**
 * @brief       If valid parameters were provided, reads in the length of data
 *              requested and places it in the passed in data buffer. 
 * @param [in]  data - Buffer in which the incoming read data is placed.   
 * @param [in]  length - Length of data to be read.   
 * @return      Status indicating if the read was successful.     
 */
static enum MDFU_TRANSPORT_RESULT Read(uint8_t* data, uint16_t length)
{
    enum MDFU_TRANSPORT_RESULT status = MDFU_TRANSPORT_INVALID_ARG;

    if (UartArgumentsValid(data, length))
    {
        status = ReadData(data, length);
    }

    return status;
}

/**
 * @brief       If valid parameters were provided, writes out the length of data
 *              requested from the passed in data buffer. 
 * @param [in]  data - Buffer containing the data bytes to be written.  
 * @param [in]  length - Length of data to be written.   
 * @return      Status indicating if the write was successful.     
 */
static enum MDFU_TRANSPORT_RESULT Write(uint8_t* data, uint16_t length)
{
    enum MDFU_TRANSPORT_RESULT status = MDFU_TRANSPORT_INVALID_ARG;
    
    if (UartArgumentsValid(data, length))
    {
        status = WriteData(data, length);
    }
    
    while (!UART1_IsTxDone())
    {
        
    }
    
    return status;
}

/**
 * @brief       Resets the UART driver when an error is detected. 
 * @param [in]  None.   
 * @return      None.     
 */
static void HandleUartError(void) 
{
    UART1_Deinitialize();
    UART1_Initialize();
}

/**
 * @brief       Performs actions required when a start of frame byte (0x56) is 
 *              received. 
 * @param [in]  None.   
 * @return      None.     
 */
static void HandleStartOfFrame(void) 
{
    isReceiveWindowOpen = true;
    isEscapedByte = false;
    commandReceiver.receiveIndex = 0U;
    commandReceiver.status = MDFU_TRANSPORT_BUSY;
}

/**
 * @brief       Performs actions required when an end of frame byte (0x9E) is 
 *              received including calculating the integrity check of the entire 
 *              frame held in the commandReceiver and verifying it matches the 
 *              value received from the host.
 * @param [in]  None.   
 * @return      None.     
 */
static void HandleEndOfFrame(void) 
{
    uint16_t calculatedFrameCheck = CalculateIntegrityCheck(commandReceiver.bufferPtr, commandReceiver.receiveIndex - FRAME_CHECK_SIZE);
    uint16_t receivedFrameCheck = 0;
    const size_t frameCheckOffset = commandReceiver.receiveIndex - FRAME_CHECK_SIZE;
    
    isReceiveWindowOpen = false;
    isEscapedByte = false;

    (void)memcpy(&receivedFrameCheck, (const void *)&commandReceiver.bufferPtr[frameCheckOffset], 2);
    
    if (commandReceiver.receiveIndex < MINIMUM_DECODED_FRAME_SIZE) 
    {
        commandReceiver.status = MDFU_TRANSPORT_FAIL_TOO_SHORT;
    } 
    else if (calculatedFrameCheck != receivedFrameCheck)
    {
        commandReceiver.status = MDFU_TRANSPORT_FAIL_INTEGRITY_CHECK_ERROR;
    }
    else
    {
        commandReceiver.status = MDFU_TRANSPORT_COMPLETE;
    }
}

/**
 * @brief       Performs actions required when an escape code byte (0xCC) is 
 *              received.
 * @param [in]  None.   
 * @return      None.     
 */
static void HandleEscapeCode(void) 
{
    isEscapedByte = true;
    commandReceiver.status = MDFU_TRANSPORT_BUSY;
}

/**
 * @brief       Performs actions required when a non-special character byte is 
 *              receive including placing the byte in the commandReceiver, if 
 *              there is space remaining.
 * @param [in]  nextByte - The data byte to be received.   
 * @return      None.     
 */
static void HandleByteReceive(uint8_t nextByte) 
{
    if (commandReceiver.receiveIndex < sizeof(receiveBuffer)) 
    {
        commandReceiver.bufferPtr[commandReceiver.receiveIndex] = nextByte;
        (commandReceiver.receiveIndex)++;
        commandReceiver.status = MDFU_TRANSPORT_BUSY;
    } 
    else 
    {
        isReceiveWindowOpen = false;
        commandReceiver.status = MDFU_TRANSPORT_FAIL_TOO_LONG;
    }
}

/**
 * @brief       Verifies UART read and write arguments are valid.
 * @param [in]  data - UART data to write out or read into. 
 * @param [in]  length - The length of data to read or write. 
 * @return      Boolean indicating if the arguments provided are valid.      
 */
static bool UartArgumentsValid(uint8_t const *data, uint16_t length)
{
    return !((length == 0U) || (data == NULL));
}

/**
 * @brief       Calls the UART driver to write out the provided data. 
 * @param [in]  data - Data to write out to the UART driver. 
 * @param [in]  length - Number of bytes in the provided data buffer to write 
 *              out.
 * @return      Status indicating if the write was successful.     
 */
static enum MDFU_TRANSPORT_RESULT WriteData(uint8_t* data, uint16_t length)
{
    uint16_t writeLength = length;
    uint8_t* writeData = data;
    enum MDFU_TRANSPORT_RESULT status = MDFU_TRANSPORT_COMPLETE;
    
    while (writeLength > 0u)
    {
        if ((UartHasAnError() == 0) && UART1_IsTxReady())
        {
            UART1_Write(*writeData++);
            writeLength--; 
        }
        else
        {
            status = MDFU_TRANSPORT_FAIL;
            HandleUartError();
            break;
        }
    }
    return status;
}

/**
 * @brief       Calls the UART driver to read in any available data. 
 * @param [in]  data - Data buffer to place available data from the UART driver.
 * @param [in]  length - Number of bytes to read.
 * @return      Status indicating if the read was successful.     
 */
static enum MDFU_TRANSPORT_RESULT ReadData(uint8_t* data, uint16_t length)
{
    enum MDFU_TRANSPORT_RESULT status = MDFU_TRANSPORT_COMPLETE;
    
    for (uint16_t byteIndex = 0; byteIndex < length; byteIndex++)
    {
        if ((UartHasAnError() == 0) && UART1_IsRxReady())
        {
            data[byteIndex] = UART1_Read();
        }
        else 
        {
            status = MDFU_TRANSPORT_FAIL;
            HandleUartError();
            break;
        }
    }
    
    return status;
}

/**
 * @brief       Helper function called by TransmitFrame. Determines the next 
 *              byte of data to transmit. If the length of data to transmit 
 *              has not yet been reached, the next byte in the transmit buffer 
 *              will be returned, otherwise the integrity check will be 
 *              returned. 
 * @param [in]  transmitBuffer - Buffer containing data to be transmitted to the 
 *              host.
 * @param [in]  sentByteCount - Number of bytes from this frame that have been 
 *              processed/written to the UART driver. 
 * @param [in]  length - Length of total number of bytes to transmit this frame.
 * @param [in]  integrityCheck - Integrity check of the frame to be transmitted. 
 * @return      The next byte to write out. 
 */
static uint8_t DetermineNextByte(uint8_t const *transmitBuffer, uint16_t sentByteCount, uint16_t length, uint16_t integrityCheck)
{
    uint8_t nextByte;

    if(sentByteCount == length)
    {
        nextByte = (uint8_t)(integrityCheck & 0x00FFU);
    }
    else if(sentByteCount == (length + 1U))
    {
        nextByte = (uint8_t)(integrityCheck >> 8);
    }
    else
    {
        nextByte = transmitBuffer[sentByteCount];
    }
    
    return nextByte;
}

/**
 * @brief       Determines if the byte provided is a reserved code. 
 * @param [in]  nextByte - Byte of data to process.
 * @return      Status indicating if the byte provided is a reserved code.     
 */
static bool IsReservedCode(uint8_t nextByte)
{
    return ((nextByte == START_OF_FRAME_CODE) || (nextByte == END_OF_FRAME_CODE) || (nextByte == ESCAPE_CODE));
}

/**
 * @brief       Checks if the next byte is a reserved code and writes the data 
 *              byte to the UART driver, adding an escape code if the byte is 
 *              a reserved character. 
 * @param [in]  nextByte - Byte of data to write. 
 * @return      Status indicating if the transmit was successful.     
 */
static enum MDFU_TRANSPORT_RESULT TransmitNextByte(uint8_t nextByte) 
{
    uint8_t byteToTransmit = nextByte;
    enum MDFU_TRANSPORT_RESULT status = MDFU_TRANSPORT_FAIL;
    
    if (IsReservedCode(byteToTransmit))
    {
        status = Write(&(uint8_t){ESCAPE_CODE}, 1U);
        if (status == MDFU_TRANSPORT_COMPLETE)
        {
            byteToTransmit = ~byteToTransmit;
        }
    }

    status = Write((uint8_t *) & byteToTransmit, 1U);
    
    return status;     
}

static bool CommandReceiverHasAnError(void) {
    return (commandReceiver.status == MDFU_TRANSPORT_FAIL) 
           || (commandReceiver.status == MDFU_TRANSPORT_FAIL_INTEGRITY_CHECK_ERROR) 
           || (commandReceiver.status == MDFU_TRANSPORT_FAIL_TOO_LONG) 
           || (commandReceiver.status == MDFU_TRANSPORT_FAIL_TOO_SHORT);
}

