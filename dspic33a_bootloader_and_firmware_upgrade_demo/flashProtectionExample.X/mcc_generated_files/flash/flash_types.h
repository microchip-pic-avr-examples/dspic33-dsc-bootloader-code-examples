
/**
 * flash Generated Driver Types Header File
 *
 * @file        flash_types.h
 *
 * @ingroup     flashdriver
 *
 * @brief       FLASH Driver using dsPIC MCUs
 *
 * @skipline @version   Firmware Driver Version 1.0.0
 *
 * @skipline @version   PLIB Version 1.0.1
 *
 * @skipline    Device : dsPIC33AK128MC106
*/

/*disclaimer*/

#ifndef FLASH_TYPES_H
#define FLASH_TYPES_H
/**
 @ingroup  flashdriver
 @brief    Defines FLASH odd address mask value
*/
#define  FLASH_ADDRESS_MASK  4U
/**
 @ingroup  flashdriver
 @brief    Defines FLASH unlock key
*/
#define  FLASH_UNLOCK_KEY  0x00AA0055
/**
 @ingroup  flashdriver
 @brief    Defines FLASH page erase opcode
*/
#define  FLASH_PAGE_ERASE_OPCODE  0x4003
/**
 @ingroup  flashdriver
 @brief    Defines FLASH word write opcode
*/
#define  FLASH_WORD_WRITE_OPCODE  0x4001
/**
 @ingroup  flashdriver
 @brief    Defines FLASH word write opcode
*/
#define  FLASH_ROW_WRITE_OPCODE  0x4002
/**
 @ingroup  flashdriver
 @brief    Defines FLASH flash page size
*/
#define  FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS  1024U
/**
 @ingroup  flashdriver
 @brief    Defines FLASH erase page mask
*/
#define  FLASH_ERASE_PAGE_MASK  (~(FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS - 1U))

typedef void (*FLASH_CALLBACK)(void *);

typedef uint32_t flash_adr_t;
typedef uint32_t flash_data_t;
typedef uint32_t flash_key_t;

/**
 @ingroup  flashdriver
 @enum     FLASH_PANEL
 @brief    This enum is be used for the flash panel numbers and maximum flash panels.
*/
enum FLASH_PANEL
{
    FLASH_PANEL_1 = 0,
    FLASH_PANEL_2 = 1,
    FLASH_PANEL_MAX_PANELS
};

/**
 @ingroup  flashdriver
 @enum     FLASH_RETURN_STATUS
 @brief    This enum is be used to return the status of write, read and erase operation.
*/
enum FLASH_RETURN_STATUS {
    FLASH_NO_ERROR,                           /**< No error occurred */
    FLASH_INVALID_ADDRESS,                    /**< Invalid address */
    FLASH_INVALID_DATA,                       /**< Invalid data */
    FLASH_WRITE_ERROR,                        /**< Write error has occurred */
    FLASH_READ_ERROR,                         /**< Read error has occurred */
    FLASH_ERASE_ERROR,                        /**< Flash erase error occurred */
    FLASH_INVALID_CALLBACK_HANDLER,           /**< Invalid parameter to operation */
    FLASH_OP_BUSY,                            /**< Flash is physically busy */
    FLASH_OP_IN_PROGRESS,                     /**< Flash operation is in progress */
    FLASH_INVALID_KEY,                        /**< Invalid NVM unlock Key  */
    FLASH_INVALID_OEPRATION,                  /**< Invalid program/erase operation (PROGOP) */
    FLASH_SECURITY_ACCESS_CONTROL_ERROR,      /**< Flash Security access control violation  */
    FLASH_PANEL_CONTROL_LOGIC_ERROR,          /**< Error reported by flash panel control logic  */
    FLASH_ROW_OP_SYSTEM_BUS_ERROR,            /**< Flash System bus error during row operation  */
    FLASH_ROW_OP_WARM_RESET_ERROR             /**< Row programming operation not completed due to warm reset */
};

#endif //FLASH_TYPES_H
