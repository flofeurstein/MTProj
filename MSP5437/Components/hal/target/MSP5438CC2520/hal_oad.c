/**************************************************************************************************
  Filename:       hal_oad.c
  Revised:        $Date: 2010-11-20 08:12:26 -0800 (Sat, 20 Nov 2010) $
  Revision:       $Revision: 24474 $

  Description:    Boot Code to support OAD on the MSP5438.


  Copyright 2004-2010 Texas Instruments Incorporated. All rights reserved.

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

/*********************************************************************
 * INCLUDES
 */

#include <msp430.h>

#include "comdef.h"
#include "hal_adc.h"
#include "hal_board_cfg.h"
#include "hal_oad.h"
#include "hal_types.h"
#include "hal_xnv.h"

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#if (!defined HAL_OAD_BL21 && !defined HAL_OAD_BL22)
#pragma location="HAL_BOOT_SEG"
void bootMain(void);
#pragma location="HAL_BOOT_SEG"
static void vddWait(uint8 vdd);
#pragma location="HAL_BOOT_SEG"
static void dl2rc(void);
#pragma location="HAL_BOOT_SEG"
static uint16 crcCalc(void);
#pragma location="HAL_BOOT_SEG"
#endif
static uint16 runPoly(uint16 crc, uint8 val);

#if (!defined HAL_OAD_BL21 && !defined HAL_OAD_BL22)
/*********************************************************************
 * @fn      bootMain
 *
 * @brief   ISR for the reset vector.
 *
 * @param   None.
 *
 * @return  None.
 *********************************************************************/
void bootMain(void)
{
  extern void _ivecRst2(void);   // oad_ivec.s43 defines this reset re-direct operation.

  HAL_BOOT_INIT();
  vddWait(VDD_MIN_RUN);

  while (1)
  {
    uint16 crc[2];

    crc[0] = *((uint16*)LO_ROM_BEG);
    crc[1] = *((uint16*)LO_ROM_BEG+1);

    if (crc[0] == crc[1])
    {
      break;
    }
    else if ((crc[0] != 0) && (crc[0] == crcCalc()))
    {
      FCTL3 = FWKEY;                   // Clear Lock bit.
      FCTL1 = FWKEY + WRT;             // Set WRT bit for write operation
      *((uint16*)LO_ROM_BEG+1) = crc[0];
      FCTL1 = FWKEY;                   // Clear WRT bit
      FCTL3 = FWKEY + LOCK;            // Set LOCK bit
    }
    else
    {
      dl2rc();
    }
  }

  _ivecRst2();
}

/*********************************************************************
 * @fn      vddWait
 *
 * @brief   Loop waiting for some minimum number of Vdd reads over the requested limit.
 *
 * @param   vdd - Vdd level to wait for.
 *
 * @return  None.
 *********************************************************************/
static void vddWait(uint8 vdd)
{
  uint8 cnt = 16;

  ADC12CTL0 = ADC12REFON | ADC12ON;
  ADC12CTL1 = ADC12SHP;
  ADC12CTL2 = 0;
  ADC12MCTL0 = ADC12SREF_1 | ADC12EOS | ADC12INCH_11;

  __delay_cycles(6000);                    // Settling time for internal reference on SmartRF05EB.

  do {
    do {
      ADC12CTL0 |= ADC12ENC + ADC12SC;     // Sampling and conversion start.
      while (ADC12IFG == 0);
    } while ((uint8)(ADC12MEM0) <= VDD_MIN_OAD);
  } while (--cnt);
 
  ADC12CTL0 = 0;                           // Disable the ADC
  ADC12CTL0 = 0;                           // Turn off reference (must be done AFTER clearing ENC).
}

/*********************************************************************
 * @fn      dl2rc
 *
 * @brief   Copy the DL image to the RC image location.
 *
 *  NOTE:   Assumes that DL image at least fills lower flash.
 *          Assumes that DL image ends on a flash page boundary.
 *
 * @param   None.
 *
 * @return  None.
 *********************************************************************/
static void dl2rc(void)
{
  uint32 addr = 0;
  uint32 addr2 = HI_ROM_BEG;
  uint16 *ptr;
  preamble_t preamble;
  uint16 buf;
  uint8 cnt = 0;

  vddWait(VDD_MIN_OAD);
  HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_DL);
  FCTL3 = FWKEY;                   // Clear Lock bit.

  for (ptr = (uint16 *)LO_ROM_BEG; ptr <= (uint16 *)LO_ROM_END ; )
  {
    FCTL1 = FWKEY + ERASE;         // Set Erase bit.
    *ptr = 0;                      // Dummy write to erase Flash segment.
    FCTL1 = FWKEY + WRT;           // Set WRT bit for write operation

    do
    {
      HalXNVRead(addr, (uint8 *)&buf, 2);
      *ptr++ = buf;
      addr += 2;
    } while (--cnt);               // Wrap a uint8 once to count 256 * 2 = 512.
  }

  for (; addr < preamble.len; )
  {
    FCTL1 = FWKEY + ERASE;         // Set Erase bit.
    __data20_write_char(addr2,0);  // Dummy write to erase Flash segment.
    FCTL1 = FWKEY + WRT;           // Set WRT bit for write operation

    do
    {
      HalXNVRead(addr, (uint8 *)&buf, 2);
      __data20_write_short(addr2, buf);
      addr2 += 2;
      addr += 2;
    } while (--cnt);               // Wrap a uint8 once to count 256 * 2 = 512.
  }

  FCTL1 = FWKEY;                   // Clear WRT bit
  FCTL3 = FWKEY + LOCK;            // Set LOCK bit
}

/*********************************************************************
 * @fn      crcCalc
 *
 * @brief   Run the CRC16 Polynomial calculation over the RC image.
 *
 * @param   None.
 *
 * @return  The CRC16 calculated.
 */
static uint16 crcCalc(void)
{
  preamble_t preamble;
  uint32 oset;
  uint8 *ptr;
  uint16 crc = 0;

  HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_RC);
  if (preamble.len > HAL_OAD_DL_SIZE)
  {
    return 0;
  }

  preamble.len = HI_ROM_BEG + preamble.len - (LO_ROM_END - LO_ROM_BEG + 1);

  // Skipping 2-byte CRC and CRC shadow at the beginning of image.
  for (ptr = (uint8 *)LO_ROM_BEG+4; ptr <= (uint8 *)LO_ROM_END; ptr++)
  {
    crc = runPoly(crc, *ptr);
  }

  for (oset = HI_ROM_BEG; oset < preamble.len; oset++)
  {
    crc = runPoly(crc, __data20_read_char(oset));
  }

  // IAR note explains that poly must be run with value zero for each byte of crc.
  crc = runPoly(crc, 0);
  crc = runPoly(crc, 0);

  return crc;
}
#endif

/*********************************************************************
 * @fn      runPoly
 *
 * @brief   Run the CRC16 Polynomial calculation over the byte parameter.
 *
 * @param   crc - Running CRC calculated so far.
 * @param   val - Value on which to run the CRC16.
 *
 * @return  crc - Updated for the run.
 */
static uint16 runPoly(uint16 crc, uint8 val)
{
  const uint16 poly = 0x1021;
  uint8 cnt;

  for (cnt = 0; cnt < 8; cnt++, val <<= 1)
  {
    if (crc & 0x8000)
    {
      crc <<= 1;
      if (val & 0x80)  crc |= 0x0001;
      crc ^= poly;
    }
    else
    {
      crc <<= 1;
      if (val & 0x80)  crc |= 0x0001;
    }
  }

  return crc;
}

/*********************************************************************
 * @fn      HalOADChkDL
 *
 * @brief   Run the CRC16 Polynomial calculation over the DL image.
 *
 * @param   dlImagePreambleOffset - Offset into the monolithic DL image to read the preamble.
 *
 * @return  SUCCESS or FAILURE.
 *********************************************************************/
uint8 HalOADChkDL(uint8 dlImagePreambleOffset)
{
  preamble_t preamble;
  uint32 oset;
  uint16 crc = 0, crc2;

  HalXNVRead(dlImagePreambleOffset, (uint8 *)&preamble, sizeof(preamble_t));

  // Run the CRC calculation over the downloaded image.
  for (oset = 4; oset < preamble.len; oset++)
  {
    uint8 buf;
    HalXNVRead(oset, &buf, 1);
    crc = runPoly(crc, buf);
  }

  // IAR note explains that poly must be run with value zero for each byte of crc.
  crc = runPoly(crc, 0);
  crc = runPoly(crc, 0);

  HalXNVRead(0, (uint8 *)&crc2, 2);  // Read the CRC from the XNV.
  return (crc2 == crc) ? SUCCESS : FAILURE;
}

#if !defined HAL_OAD_BL21
/*********************************************************************
 * @fn      HalOADInvRC
 *
 * @brief   Invalidate the active image so that the boot code will instantiate the DL image on the
 *          next reset.
 *
 * @param   None.
 *
 * @return  None.
 *********************************************************************/
void HalOADInvRC(void)
{
  // Save interrupt and watchdog settings.
  istate_t ist = __get_interrupt_state();
  uint8 wdt = WDTCTL & 0xFF;
  WDTCTL = WDTPW + WDTHOLD;            // Stop watchdog per data sheet.
  __disable_interrupt();               // Stop interrupts to insure sequential writes in time.

  FCTL3 = FWKEY;                   // Clear Lock bit.
  FCTL1 = FWKEY + WRT;             // Set WRT bit for write operation
  *((uint16 *)LO_ROM_BEG) = 0x0000;
  FCTL1 = FWKEY;                   // Clear WRT bit
  FCTL3 = FWKEY + LOCK;            // Set LOCK bit

  WDTCTL = WDTPW + wdt;                // Restore watchdog setting.
  __set_interrupt_state(ist);          // Restore interrupts setting.
}
#endif

#if HAL_OAD_XNV_IS_INT
/*********************************************************************
 * @fn      HalOADAvail
 *
 * @brief   Determine the space available for downloading an image.
 *
 * @param   None.
 *
 * @return  Number of bytes available for storing an OAD image.
 *********************************************************************/
uint32 HalOADAvail(void)
{
  /*
  preamble_t preamble;

  HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_RC);
  return HAL_OAD_DL_MAX - preamble.len;
   */
  return HAL_OAD_DL_MAX - HAL_OAD_DL_OSET;
}
#elif HAL_OAD_XNV_IS_SPI
uint32 HalOADAvail(void)
{
  return HAL_OAD_DL_MAX - HAL_OAD_DL_OSET;
}
#else
#error Invalid Xtra-NV for OAD.
#endif

/*********************************************************************
 * @fn      HalOADRead
 *
 * @brief   Read from the storage medium according to image type.
 *
 * @param   oset - Offset into the monolithic image.
 * @param   pBuf - Pointer to the buffer in which to copy the bytes read.
 * @param   len - Number of bytes to read.
 * @param   type - Which image: HAL_OAD_RC or HAL_OAD_DL.
 *
 * @return  None.
 *********************************************************************/
void HalOADRead(uint32 oset, uint8 *pBuf, uint16 len, image_t type)
{
  if (HAL_OAD_DL == type)
  {
    HalXNVRead(oset, pBuf, len);
  }
  else
  {
    istate_t ss = __get_interrupt_state();
    __disable_interrupt();

    if ((oset + LO_ROM_BEG) > LO_ROM_END)  // Read from active image in high flash.
    {
      oset = HI_ROM_BEG + oset - (LO_ROM_END - LO_ROM_BEG + 1);
    }
    else                                   // Read from active image in low flash.
    {
      uint8 *ptr = (uint8 *)((uint16)oset + LO_ROM_BEG);

      while (len)
      {
        // If a read crosses from low to high flash, break and fall into high flash read loop.
        if (ptr > (uint8 *)LO_ROM_END)
        {
          oset = HI_ROM_BEG;
          break;
        }
        *pBuf++ = *ptr++;
        len--;
      }
    }

    while (len--)
    {
      *pBuf++ = __data20_read_char(oset++);
    }

    __set_interrupt_state(ss);
  }
}

/*********************************************************************
 * @fn      HalOADWrite
 *
 * @brief   Write to the storage medium according to the image type.
 *
 *  NOTE:   Destructive write on page boundary! When writing to the first flash word
 *          of a page boundary, the page is erased without saving/restoring the bytes not written.
 *          Writes anywhere else on a page assume that the location written to has been erased.
 *
 * @param   oset - Offset into the monolithic image, aligned to HAL_FLASH_WORD_SIZE.
 * @param   pBuf - Pointer to the buffer in from which to write.
 * @param   len - Number of bytes to write. If not an even multiple of HAL_FLASH_WORD_SIZE,
 *                remainder bytes are overwritten with garbage.
 * @param   type - Which image: HAL_OAD_RC or HAL_OAD_DL.
 *
 * @return  None.
 *********************************************************************/
void HalOADWrite(uint32 oset, uint8 *pBuf, uint16 len, image_t type)
{
  if (HAL_OAD_DL == type)
  {
    HalXNVWrite(oset, pBuf, len);
  }
}

/**************************************************************************************************
*/
