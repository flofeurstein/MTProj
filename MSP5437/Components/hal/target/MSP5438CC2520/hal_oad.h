/**************************************************************************************************
  Filename:       hal_oad.h
  Revised:        $Date: 2010-11-20 08:12:26 -0800 (Sat, 20 Nov 2010) $
  Revision:       $Revision: 24474 $

  Description:    This module defines optionally-compiled Boot Code parameters for the MSP5438.

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
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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

#ifndef HAL_OAD_H
#define HAL_OAD_H

/*********************************************************************
 * INCLUDES
 */

#include "hal_board_cfg.h"
#include "hal_types.h"

/*********************************************************************
 * MACROS
 */

#if !defined HAL_OAD_BOOT_CODE
#define HAL_OAD_BOOT_CODE  FALSE
#endif

#define PACK_1 _Pragma("pack(1)")
#pragma diag_suppress=Pa039

/* ----------- Boot Code Board Init - OAD Support ---------- */
#define HAL_BOOT_INIT()                            \
st(                                                \
  WDTCTL = WDTPW | WDTHOLD;                        \
  asm("MOV #5BFCh, SP");                           \
)

/*********************************************************************
 * CONSTANTS
 */

// Placement controlled by oad.xcl.
#define HAL_OAD_RC_START           0x5C00
#define HAL_OAD_CRC_ADDR           0x5C00
#define HAL_OAD_CRC_OSET          (HAL_OAD_CRC_ADDR - HAL_OAD_RC_START)

#define HAL_OAD_CHKDL_ADDR         0xFD34
#define HAL_OAD_INVRC_ADDR         0xFDC2
#define HAL_OAD_AVAIL_ADDR         0xFDFC
#define HAL_OAD_READ_ADDR          0xFE04
#define HAL_OAD_WRITE_ADDR         0xFE70

/* Note that corresponding changes must be made to oad.xcl when changing the source of Xtra-NV.
 * When using internal flash for XNV, (HAL_OAD_BOOT_PG_CNT + HAL_NV_PAGE_CNT) must be even.
 */
#define HAL_OAD_XNV_IS_INT         TRUE
#define HAL_OAD_XNV_IS_SPI        !HAL_OAD_XNV_IS_INT

#define HAL_OAD_BOOT_PG_CNT        3

/* To reduce the binary image size due to padding un-used code space, reduce HAL_OAD_DL_SIZE
 * to the minimum required for your Application and make the corresponding changes to oad.xcl.
 * This size must be an even multiple of HAL_FLASH_PAGE_SIZE.
 */
#if HAL_OAD_XNV_IS_SPI
#error Only Internal XNV is supported.
#else
#define HAL_OAD_DL_MAX    0x45C00
#if defined HAL_OAD_BL21
// Forced to split a page boundary because of bug in pre-2.3 boot loader.
#define HAL_OAD_DL_OSET   0x26700
#define HAL_OAD_DL_SIZE   0x1E400
#elif defined HAL_OAD_BL22
// Forced to split a page boundary because of bug in pre-2.3 boot loader.
#define HAL_OAD_DL_OSET   0x27700
#define HAL_OAD_DL_SIZE   0x1E500
#else
#define HAL_OAD_DL_OSET   0x27600
// Floor page bound of: ((0x40000-((HAL_NV_PAGE_CNT+HAL_OAD_BOOT_PG_CNT)*HAL_FLASH_PAGE_SIZE)) / 2).
#define HAL_OAD_DL_SIZE   0x1E400
#endif
#endif

// To run OAD with the legacy ZOAD.exe PC tool, place the preamble in this legacy location.
#define PREAMBLE_OFFSET            0x8C

#define LO_ROM_BEG                 0x5C00
#if defined HAL_OAD_BL21
#define LO_ROM_END                 0xE9FF
#else
#define LO_ROM_END                 0xC9FF
#endif
#define HI_ROM_BEG                 0x10000

#if HAL_OAD_XNV_IS_INT
#define VDD_MIN_OAD  VDD_MIN_NV
#else
#error MSP5438 OAD by external NV not characterized.
#endif

/*********************************************************************
 * TYPEDEFS
 */

typedef enum {
  HAL_OAD_RC,  /* Run code / active image.          */
  HAL_OAD_DL   /* Downloaded code to be activated later. */
} image_t;

PACK_1
typedef struct {
  uint8 magic[2];
  uint32 len;
  uint16  vers;
  uint16  manu;
  uint16  prod;
} preamble_t;

/*********************************************************************
 * FUNCTIONS
 */

#if !defined ZPORT
#if (defined HAL_OAD_BL21 || defined HAL_OAD_BL22)
uint8 HalOADChkDL(uint8 dlImagePreambleOffset);
uint32 HalOADAvail(void);
void HalOADRead(uint32 oset, uint8 *pBuf, uint16 len, image_t type);
void HalOADWrite(uint32 oset, uint8 *pBuf, uint16 len, image_t type);
void HalOADInvRC(void);
#elif HAL_OAD_BOOT_CODE
#pragma location="HAL_OAD_CHKDL_SEG"
uint8 HalOADChkDL(uint8 dlImagePreambleOffset);
#pragma location="HAL_OAD_INVRC_SEG"
void HalOADInvRC(void);
#pragma location="HAL_OAD_AVAIL_SEG"
uint32 HalOADAvail(void);
#pragma location="HAL_OAD_READ_SEG"
void HalOADRead(uint32 oset, uint8 *pBuf, uint16 len, image_t type);
#pragma location="HAL_OAD_WRITE_SEG"
void HalOADWrite(uint32 oset, uint8 *pBuf, uint16 len, image_t type);
#else
uint8 (*HalOADChkDL)(uint8 dlImagePreambleOffset) =
 (uint8 (*)(uint8 dlImagePreambleOffset))HAL_OAD_CHKDL_ADDR;
void (*HalOADInvRC)(void) = (void (*)(void))HAL_OAD_INVRC_ADDR;
uint32 (*HalOADAvail)(void) = (uint32 (*)(void))HAL_OAD_AVAIL_ADDR;
void (*HalOADRead)(uint32 oset, uint8 *pBuf, uint16 len, image_t type) =
  (void (*)(uint32 oset, uint8 *pBuf, uint16 len, image_t type))HAL_OAD_READ_ADDR;
void (*HalOADWrite)(uint32 oset, uint8 *pBuf, uint16 len, image_t type) =
  (void (*)(uint32 oset, uint8 *pBuf, uint16 len, image_t type))HAL_OAD_WRITE_ADDR;
#endif
#endif
#endif
