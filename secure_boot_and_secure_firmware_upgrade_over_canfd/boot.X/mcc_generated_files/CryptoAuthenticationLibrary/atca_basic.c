/* 
 * This file intentionally cleared. To obtain the original source, download the MCC CryptoAuthentication Library and Trust Anchor Library. 
 * See the CryptoAuthentication Library Release Notes for download instructions.
 * 
 * CryptoAuthentication Library: https://onlinedocs.microchip.com/pr/GUID-7F2639F3-1541-4BFC-A031-9A718BFFC502-en-US-16/index.html?GUID-16C4211F-EC26-4980-8F90-CCEFAB464D3C
 * Trust Anchor Library: https://onlinedocs.microchip.com/pr/GUID-7F2639F3-1541-4BFC-A031-9A718BFFC502-en-US-16/index.html?GUID-7D69101E-EA71-47BE-B794-7F2CA6363B2C
 */ 
#include "cryptoauthlib.h"
#include <stddef.h>

ATCA_STATUS atcab_init( void* i)
{
    return !ATCA_SUCCESS;
}

void atcab_lock_data_slot(int i)
{
    return;
}

ATCA_STATUS atcab_verify_extern(uint8_t *a, uint8_t *b, const uint8_t *c, bool *d)
{
    return !ATCA_SUCCESS;
}

ATCAIfaceCfg* atgetifacecfg(ATCAIface i)
{
    return NULL;
}