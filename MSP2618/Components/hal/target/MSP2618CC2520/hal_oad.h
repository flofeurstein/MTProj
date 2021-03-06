/**************************************************************************************************
  Filename:       hal_oad.h
  Revised:        $Date: 2010-11-20 08:12:26 -0800 (Sat, 20 Nov 2010) $
  Revision:       $Revision: 24474 $

  Description:    This module defines optionally-compiled Boot Code parameters for the MSP2618.

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
  extern void TI_SetDCO_Boot(int delta);           \
  WDTCTL = WDTPW | WDTHOLD;                        \
  asm("MOV #30FCh, SP");                           \
                                                   \
  /* Setup the GPIOs for DCO calibration */        \
  /* P1.1 and P1.4 outputs. */                     \
  P1DIR |= 0x12;                                   \
                                                   \
  /* P1.4 SMCLK output */                          \
  P1SEL |= 0x10;                                   \
                                                   \
  /* P2.0 output */                                \
  P2DIR |= 0x01;                                   \
                                                   \
  /* P2.0 ACLK output */                           \
  P2SEL |= 0x01;                                   \
                                                   \
  /* delay for ACLK startup */                     \
  {                                                \
    uint16 i;                                      \
    for (i=0; i<0xFFFF; i++) {};                   \
  }                                                \
                                                   \
  TI_SetDCO_Boot(TI_DCO_6MHZ);                     \
  /* Turn on the timer clock; keep 32KHz running */\
  __bis_SR_register(SCG0);                         \
  /* Set flash timing to 6 MHz / 24 -> 250 kHz. */ \
  FCTL2 = FWKEY + FSSEL_1 + FN4 + FN3;             \
                                                   \
  /* Initialize SPI that's shared by NV and LCD.*/ \
  UCB0CTL1 |= UCSWRST; \
  UCB0CTL0 |= UCMST | UCSYNC | UCCKPH | UCMSB; \
  /* MSB, Master mode, Sync mode, Data capture on the first UCLK edge. */ \
  UCB0CTL1 |= UCSSEL1;  /* SMCLK */ \
  UCB0BR0  = 4; \
  UCB0CTL1 &= ~UCSWRST; \
  P3SEL |= BV(1)|BV(2)|BV(3); \
  P6OUT |= BV(2) | BV(5); \
  P6DIR |= BV(2) | BV(3) | BV(4) | BV(5); \
)

/*********************************************************************
 * CONSTANTS
 */

// Placement controlled by oad.xcl.
#define HAL_OAD_RC_START           0x3200
#define HAL_OAD_CRC_ADDR           0x3200
#define HAL_OAD_CRC_OSET          (HAL_OAD_CRC_ADDR - HAL_OAD_RC_START)

#define HAL_OAD_CHKDL_ADDR         0xFDF8
#define HAL_OAD_INVRC_ADDR         0xFE86
#define HAL_OAD_AVAIL_ADDR         0xFEC0
#define HAL_OAD_READ_ADDR          0xFEC6
#define HAL_OAD_WRITE_ADDR         0xFF32

/* Note that corresponding changes must be made to oad.xcl when changing the source of Xtra-NV.
 * When using internal flash for XNV, (HAL_OAD_BOOT_PG_CNT + HAL_NV_PAGE_CNT) must be even.
 */
#define HAL_OAD_XNV_IS_INT         FALSE
#define HAL_OAD_XNV_IS_SPI        !HAL_OAD_XNV_IS_INT

#define HAL_OAD_BOOT_PG_CNT        3.5

/* To reduce the binary image size due to padding un-used code space, reduce HAL_OAD_DL_SIZE
 * to the minimum required for your Application and make the corresponding changes to oad_app.xcl.
 * This size must be an even multiple of HAL_FLASH_PAGE_SIZE.
 */
#if HAL_OAD_XNV_IS_SPI
#define HAL_OAD_DL_MAX    0x40000
#define HAL_OAD_DL_SIZE  (0x1CF00-((HAL_NV_PAGE_CNT+HAL_OAD_BOOT_PG_CNT)*HAL_FLASH_PAGE_SIZE))
#define HAL_OAD_DL_OSET   0x0  // Configurable offset into an external NV.
#else
#error Only SPI XNV is suppported.
#endif

// To run OAD with the legacy ZOAD.exe PC tool, place the preamble in this legacy location.
#define PREAMBLE_OFFSET            0x8C

#define LO_ROM_BEG                 0x3200
#define LO_ROM_END                 0xE1FF
#define HI_ROM_BEG                 0x10000

#if HAL_OAD_XNV_IS_INT
#error MSP2618 OAD by internal NV not characterized.
#else
#define VDD_MIN_OAD  VDD_MIN_XNV
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
#if HAL_OAD_BOOT_CODE
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
