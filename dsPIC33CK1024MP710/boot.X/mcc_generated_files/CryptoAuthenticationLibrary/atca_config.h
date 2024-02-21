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

/* Auto-generated config file atca_config.h */
#ifndef ATCA_CONFIG_H
#define ATCA_CONFIG_H

/* Included HALS */
#define ATCA_HAL_SPI

/* Included device support */
#define ATCA_TA100_SUPPORT

/** Define if cryptoauthlib is to use the maximum execution time method */
#define ATCA_NO_POLL

/* \brief How long to wait after an initial wake failure for the POST to
 *         complete.
 * If Power-on self test (POST) is enabled, the self test will run on waking
 * from sleep or during power-on, which delays the wake reply.
 */
#ifndef ATCA_POST_DELAY_MSEC
#define ATCA_POST_DELAY_MSEC 25
#endif

/***************** Diagnostic & Test Configuration Section *****************/

/** Enable debug messages */
/* Enable debug printing by defining: #define ATCA_PRINTF */

/** Enable to build in test hooks */
/* Enable built in test hooks by defining: #define ATCA_TESTS_ENABLED */

/******************** Features Configuration Section ***********************/

/** Additional Runtime Configuration */
#define ATCA_LIBRARY_CONF  "@ATCA_LIBRARY_CONF@"

/******************** Packet Size Configuration Section *************************/

/** Provide Maximum packet size for the command to be sent and received */
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE         267
#endif

/******************** Platform Configuration Section ***********************/

/** Define if the library is not to use malloc/free */
#define ATCA_NO_HEAP

#define atca_delay_ms   hal_delay_ms
#define atca_delay_us   hal_delay_us

#endif // ATCA_CONFIG_H
