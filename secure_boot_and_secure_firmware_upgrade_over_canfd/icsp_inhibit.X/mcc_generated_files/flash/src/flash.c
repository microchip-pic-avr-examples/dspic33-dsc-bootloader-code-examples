/**
 * FLASH Generated Driver Source File
 *
 * @file      flash.c
 *
 * @ingroup   Flash Driver
 *
 * @brief     This is the generated driver source file for FLASH driver
 *
 * @skipline @version   Firmware Driver Version 1.1.0
 *
 * @skipline @version   PLIB Version 3.2.1
 *
 * @skipline  Device : dsPIC33CK1024MP710
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <stddef.h>
#include "../flash.h"

// Section: Driver Interface
const struct FLASH_INTERFACE flash = {
    .PageErase = FLASH_PageErase,
    .Write = FLASH_WordWrite,
    .RowWrite = FLASH_RowWrite,
    .Read = FLASH_Read,
    .PageAddressGet = FLASH_ErasePageAddressGet,
    .PageOffsetGet = FLASH_ErasePageOffsetGet,
    .NonBlockingPageErase = NULL,
    .NonBlockingBulkErase = NULL,
    .NonBlockingRead = NULL,
    .NonBlockingWordWrite = NULL,
    .NonBlockingRowWrite = NULL,
    .NonBlockingPolledPageErase = NULL,
    .NonBlockingPolledBulkErase = NULL,
    .NonBlockingPolledRead = NULL,
    .NonBlockingPolledWordWrite = NULL,
    .NonBlockingPolledRowWrite = NULL,
};

enum FLASH_RETURN_STATUS FLASH_PageErase(uint32_t flashAddress, flash_key_t unlockKey)
{
    enum FLASH_RETURN_STATUS flashReturnStatus = FLASH_NO_ERROR;

    if (0U != ((uint16_t)flashAddress & ~FLASH_ERASE_PAGE_MASK))
    {
        flashReturnStatus = FLASH_INVALID_ADDRESS;
    }
    if(FLASH_INVALID_ADDRESS != flashReturnStatus)
    {
        bool status = FLASH_ErasePage(flashAddress, unlockKey);
        if(!status)
        {
            flashReturnStatus = FLASH_WRITE_ERROR;
        }
    }
    return flashReturnStatus;
}


enum FLASH_RETURN_STATUS FLASH_WordWrite(flash_adr_t flashAddress, flash_data_t *data, flash_key_t unlockKey)
{
    enum FLASH_RETURN_STATUS flashReturnStatus = FLASH_NO_ERROR;
    if (NULL == data)
    {
        flashReturnStatus = FLASH_INVALID_DATA;
    }
    else if (0U != ((uint16_t)flashAddress & FLASH_ODD_ADDRESS_MASK))
    {
        flashReturnStatus = FLASH_INVALID_ADDRESS;
    }
    else
    {
	    flashReturnStatus = FLASH_WriteDoubleWord24(flashAddress, *data, *(data+1), unlockKey) ? FLASH_NO_ERROR : FLASH_WRITE_ERROR;
    }
    return flashReturnStatus;
}

enum FLASH_RETURN_STATUS FLASH_RowWrite(flash_adr_t flashAddress, flash_data_t *data, flash_key_t unlockKey)
{
    enum FLASH_RETURN_STATUS flashReturnStatus = FLASH_NO_ERROR;
    if (NULL == data)
    {
        flashReturnStatus = FLASH_INVALID_DATA;
    }
    else if (0U != ((uint16_t)flashAddress & FLASH_ODD_ADDRESS_MASK))
    {
        flashReturnStatus = FLASH_INVALID_ADDRESS;
    }
    else
    {
        flashReturnStatus =  FLASH_WriteRow24(flashAddress, data, unlockKey) ? FLASH_NO_ERROR : FLASH_WRITE_ERROR;
    }
    return flashReturnStatus;
}

enum FLASH_RETURN_STATUS FLASH_Read(flash_adr_t flashAddress, size_t count, flash_data_t *data)
{
    enum FLASH_RETURN_STATUS flashReturnStatus = FLASH_NO_ERROR;
    if ((NULL == data) || (0 == count))
    {
        flashReturnStatus = FLASH_INVALID_DATA;
    }
    else if (0U != ((uint16_t)flashAddress & FLASH_ODD_ADDRESS_MASK))
    {
        flashReturnStatus = FLASH_INVALID_ADDRESS;
    }
    else
    {
        do
        {
            *data++ = FLASH_ReadWord24(flashAddress);
            flashAddress = flashAddress + FLASH_PC_UNIT_SIZE;
        } while(--count);
    }
    return flashReturnStatus;
}
