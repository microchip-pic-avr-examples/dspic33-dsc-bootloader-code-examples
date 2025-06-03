/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    cam_hash.h

  Summary:
    Crypto Framework Library interface file for CAM Hashing.

  Description:
    This header file contains the interface that make up the CAM Hashing hardware
    driver for the following families of Microchip microcontrollers:
    dsPIC33AK with Crypto Accelerator Module.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
//DOM-IGNORE-END

#ifndef CAM_HASH_H
#define	CAM_HASH_H

#ifdef	__cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include "cam_version.h"

// *****************************************************************************
// *****************************************************************************
// Section: HASH Common Data Types
// *****************************************************************************
// *****************************************************************************

/**
 * @brief Defines HASH error codes.
 */
typedef enum HASH_ERROR
{
    HASH_NO_ERROR = 0,
    HASH_CONTEXT_ERROR = 1,
    HASH_INVALID_MODE_ERROR = 2,
    HASH_INITIALIZE_ERROR = 6,
    HASH_READ_ERROR = 7,
    HASH_UPDATE_ERROR = 8,
    HASH_STATE_ERROR = 9,
    HASH_EXECUTE_ERROR = 10,
    HASH_OPERATION_IS_NOT_ACTIVE = 11,
    HASH_OPERATION_IS_ACTIVE = 12

} HASH_ERROR;

/**
 * @brief Defines the allowed HASH modes.
 */
typedef enum HASHCON_MODE
{
    MODE_SHA1   = 1,
    MODE_SHA224 = 2,
    MODE_SHA256 = 3,
    MODE_SHA384 = 4,
    MODE_SHA512 = 5
} HASHCON_MODE;

// *****************************************************************************
// *****************************************************************************
// Section: SHA Common Interface
// *****************************************************************************
// *****************************************************************************

/**
 * @brief The ISR processor for HASH operations.
 **/
void DRV_CRYPTO_HASH_IsrHelper(void);

/** @brief Initialize a new hash operation.
 *  @param contextData A pointer to the context block that contains the hash context data.
 *  @param mode The HASHCON_MODE mode to use.
 *  @return HASH_NO_ERROR on success, HASH_CONTEXT_ERROR / HASH_INVALID_MODE_ERROR /
 *          HASH_INITIALIZE_ERROR on failure.
 *  @note Reinitializing a HASH operation in progress will void the previous operation.
 **/
HASH_ERROR DRV_CRYPTO_HASH_Initialize(void *contextData, HASHCON_MODE mode);

/**
 *  @brief Add data to a hash operation.
 *  @param contextData A pointer to the context block that contains the hash context data.
 *  @param data The data to add to the operation.
 *  @param dataLength The length of the data.
 *  @return HASH_NO_ERROR on success, HASH_CONTEXT_ERROR / HASH_INITIALIZE_ERROR /
 *          HASH_STATE_ERROR / HASH_UPDATE_ERROR on failure.
 *  @note A HASH operation must be in progress (initialized by DRV_CRYPTO_HASH_Initialize()).
 **/
HASH_ERROR DRV_CRYPTO_HASH_Update(void *contextData, uint8_t *data, uint32_t dataLength);

/**
 *  @brief Execute a hash operation to generate the final digest.
 *  @param contextData A pointer to the context block that contains the hash context data.
 *  @param digest Buffer to contain the digest.
 *  @param digestLength The size of the digest buffer.
 *  @return HASH_NO_ERROR on success, HASH_CONTEXT_ERROR / HASH_INITIALIZE_ERROR /
 *          HASH_STATE_ERROR / HASH_EXECUTE_ERROR on failure.
 *  @note A hash operation must be in progress (initialized by DRV_HASH_CRYPTO_Initialize()).
 **/
HASH_ERROR DRV_CRYPTO_HASH_Final(void *contextData, uint8_t * digest, uint32_t digestLength);

/**
 *  @brief Execute a single hash operation to generate a digest.
 *  @param contextData A pointer to the context block that contains the hash context data.
 *  @param mode The HASHCON_MODE mode to use.
 *  @param data The data to add to the operation.
 *  @param dataLength The length of the data.
 *  @param digest Buffer to contain the digest.
 *  @param digestLength The size of the digest buffer.
 *  @return HASH_NO_ERROR on success, HASH_CONTEXT_ERROR / HASH_INITIALIZE_ERROR /
 *          HASH_STATE_ERROR / HASH_EXECUTE_ERROR on failure.
 *  @note A hash operation must be in progress (initialized by DRV_HASH_CRYPTO_Initialize()).
 **/
HASH_ERROR DRV_CRYPTO_HASH_Digest(void *contextData, HASHCON_MODE mode,
    uint8_t *data, uint32_t dataLength,
    uint8_t * digest, uint32_t digestLength);

/**
 *  @brief Returns whether a hash operation is active.
 *  @param contextData A pointer to the context block that contains the hash context data.
 *  @param active Pointer to value to contain the active/inactive status.
 *  @return HASH_NO_ERROR on success, HASH_CONTEXT_ERROR / HASH_INITIALIZE_ERROR on failure.
 *  @note This sets the value pointed to by \p active to HASH_OPERATION_IS_ACTIVE
 *        or HASH_OPERATION_IS_NOT_ACTIVE.
 **/
HASH_ERROR DRV_CRYPTO_HASH_IsActive(void *contextData, HASH_ERROR *active);

/**
 *  @brief Get the size of a hash context block.
 *  @param contextData A pointer to the context block that contains the hash context data.
 *  @return Size of the hash context block in bytes.
 *  @note A NULL \p contextData pointer will return the maximum context block size.
 **/
uint32_t DRV_CRYPTO_HASH_GetContextSize(void *contextData);

#ifdef	__cplusplus
}
#endif

#endif	/* CAM_HASH_H */
