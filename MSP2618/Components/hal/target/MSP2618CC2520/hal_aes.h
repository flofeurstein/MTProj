/**************************************************************************************************
  Filename:       hal_aes.h
  Revised:        $Date: 2010-01-08 13:24:55 -0800 (Fri, 08 Jan 2010) $
  Revision:       $Revision: 21463 $

  Description:    Support for Hardware AES encryption.

  Copyright 2008-2010 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef     HAL_AES_H_
#define     HAL_AES_H_

#include "ZComDef.h"

#define     STATE_BLENGTH   16      // Number of bytes in State
#define     KEY_BLENGTH     16      // Number of bytes in Key
#define     KEY_EXP_LENGTH  176     // Nb * (Nr+1) * 4

extern void HalAesInit( void );
extern void (*pSspAesEncrypt)( uint8 *, uint8 * );
extern void ssp_HW_KeyInit (uint8 *);
extern void sspKeyExpansion (uint8 *, uint8 *);

extern void sspAesEncryptHW (uint8 *, uint8 *);
extern void sspAesEncryptKeyExp (uint8 *, uint8 *);
extern void sspAesEncryptBasic (uint8 *, uint8 *);
extern void sspAesEncrypt( uint8 *key, uint8 *buf );

#endif  // HAL_AES_H_
