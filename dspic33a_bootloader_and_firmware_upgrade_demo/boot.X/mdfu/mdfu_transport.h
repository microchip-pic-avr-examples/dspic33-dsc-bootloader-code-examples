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
#ifndef MDFU_TRANSPORT_H
#define MDFU_TRANSPORT_H

#include <stdint.h>

enum MDFU_TRANSPORT_RESULT
{
    MDFU_TRANSPORT_COMPLETE,
    MDFU_TRANSPORT_FAIL,
    MDFU_TRANSPORT_BUSY,
    MDFU_TRANSPORT_INVALID_ARG,
};

/**
 * @brief    Initializes the transport layer
 * @return   None
 */
void MDFU_TransportInitialize(void);

/**
 * @brief    Continues transfers (transmit or receive) for non-blocking
 *           implementations.
 * @return   None
 */
void MDFU_TransportTasks(void);

/**
 * @brief    Transmits a response frame to the host. This is to be called by the client's MDFU Command Processor Layer when either 
 *           a response is ready to be sent or if a frame needs to be resent.   
 * @param [in]  transmitBuffer - Pointer to the buffer being transmitted
 * @param [in]  length - Length of the response being sent
 * @return   Status indicating if the transmission was successful and if not, what error occurred  
 */
enum MDFU_TRANSPORT_RESULT MDFU_TransportTransmitFrame(uint8_t* transmitBuffer, uint16_t length);

/**
 * @brief    Returns the status of the current transmission.  Returns  MDFU_TRANSPORT_COMPLETE
 *           if no transmission has been requested since system start.
 * @return   MDFU_TRANSPORT_RESULT indicating the transmission status
 */
enum MDFU_TRANSPORT_RESULT MDFU_TransportTransmitStatusGet(void);

/**
 * @brief    Receives and verifies commands generated by the host's Firmware Update Protocol Layer. This is to be called by the 
 *           client's MDFU Command Processor Layer when processing the next command in order to load the buffer with the current frame, 
 *           if available. If no frame is available or there is another impediment within the Transport Layer (i.e. a frame is still
 *           being received), then the appropriate MDFU_TRANSPORT_RESULT response will be sent and the client MDFU Command Processor 
 *           can proceed accordingly. 
 * @param [in] frameBuffer - Pointer to the receive frame.  Must be at least MDFU_COMMAND_OVERHEAD + MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH in length.
 * @param [in] frameBufferLength - Length of the frameBuffer, Must be at least MDFU_COMMAND_OVERHEAD + MDFU_CONFIG_MAX_COMMAND_DATA_LENGTH in length.
 * @param [out] frameLength - Length of the frame received.  This is the amount of data written to the frameBuffer
 * @return   Status indicating if the receive was successful and if not, what error occurred 
 */
enum MDFU_TRANSPORT_RESULT MDFU_TransportReceiveFrame(uint8_t* frameBuffer, size_t frameBufferLength, size_t* frameLength);

#endif 
