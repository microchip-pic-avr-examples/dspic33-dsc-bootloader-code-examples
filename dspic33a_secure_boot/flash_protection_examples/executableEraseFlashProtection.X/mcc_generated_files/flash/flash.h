
/**
 * FLASH Generated Driver Header File
 *
 * @file         flash.h
 *
 * @ingroup      flashdriver
 *
 * @brief        FLASH driver using dsPIC MCUs
 *
 * @skipline @version   Firmware Driver Version 1.0.0
 *
 * @skipline @version   PLIB Version 1.0.2
 *
 * @skipline     Device : dsPIC33AK512MPS512
*/

/*disclaimer*/

#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "flash_interface.h"
/**
 @ingroup   flashdriver
 @brief     Structure object of type FLASH_INTERFACE assigned with name
            displayed in the Melody Driver User interface. A structure pointer can be used to achieve portability
            across the FLASH having same interface structure.
*/
extern const struct FLASH_INTERFACE flash;

/**
 * @ingroup flashdriver
 * @brief This function erases a page.
 * @param[in]  flashAddress : Flash address
 * @param[in]  unlockKey : Pointer of the data to be written
 * @return  FLASH_RETURN_STATUS: returns FLASH_NO_ERROR if operation is successful , else returns errors like FLASH_INVALID_ADDRESS, FLASH_WRITE_ERROR
 */
enum FLASH_RETURN_STATUS FLASH_PageErase(flash_adr_t flashAddress, flash_key_t unlockKey);


/**
 * @ingroup flashdriver
 * @brief This function writes the specified data to the specified address.
 * @param[in]  flashAddress : Flash address
 * @param[in]  data : Pointer of the data to be written.
 *  This will write the 4 WORDS pointed to by the pointer.
 * @return FLASH_RETURN_STATUS: returns FLASH_NO_ERROR if operation is successful , else returns errors like FLASH_INVALID_ADDRESS, FLASH_INVALID_DATA, FLASH_WRITE_ERROR
 */
enum FLASH_RETURN_STATUS FLASH_WordWrite(flash_adr_t flashAddress, flash_data_t *data, flash_key_t unlockKey);

/**
 * @ingroup flashdriver
 * @brief This function writes the specified data to the specified address.
 * @param[in]  flashAddress : Flash address
 * @param[in]  data : Pointer of the data to be written
 * @return FLASH_RETURN_STATUS: returns FLASH_NO_ERROR if operation is successful , else returns errors like FLASH_INVALID_ADDRESS, FLASH_INVALID_DATA, FLASH_WRITE_ERROR
 */
enum FLASH_RETURN_STATUS FLASH_RowWrite(flash_adr_t flashAddress, flash_data_t *data, flash_key_t unlockKey);

/**
 * @ingroup flashdriver
 * @brief This function reads the data from the specified address.
 * @param[in]  flashAddress : Flash address
 * @param[out]  data : Pointer to read the data
 * @return  FLASH_RETURN_STATUS: returns FLASH_NO_ERROR if operation is successful , else returns errors like FLASH_INVALID_ADDRESS, FLASH_INVALID_DATA
 */
enum FLASH_RETURN_STATUS FLASH_Read(flash_adr_t flashAddress, size_t count, flash_data_t *data);

/**
 * @ingroup flashdriver
 * @brief This function returns the offest from page start.
 * @param[in]  flashAddress : Flash address
 * @return  FLASH_RETURN_STATUS: returns the flash offset from page start address
 */
uint16_t FLASH_ErasePageOffsetGet(flash_adr_t flashAddress);


/**
 * @ingroup flashdriver
 * @brief This function returns the offest from page start.
 * @param[in]  flashAddress : Flash address
 * @return  FLASH_RETURN_STATUS: returns page start address for the given address
 */
uint32_t FLASH_ErasePageAddressGet(flash_adr_t flashAddress);


#endif    /* FLASH_H */
