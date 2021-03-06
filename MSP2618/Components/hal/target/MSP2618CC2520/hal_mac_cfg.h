/**************************************************************************************************
  Filename:       hal_mac_cfg.h
  Revised:        $Date: 2011-05-09 13:22:48 -0700 (Mon, 09 May 2011) $
  Revision:       $Revision: 25921 $

  Description:    Describe the purpose and contents of the file.


  Copyright 2008-2011 Texas Instruments Incorporated. All rights reserved.

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

#ifndef HAL_MAC_CFG_H
#define HAL_MAC_CFG_H


/*
 *   Board Configuration File for low-level MAC
 *  --------------------------------------------
 *   Manufacturer : Texas Instruments
 *   Part Number  : MSP2618CC2520 on SmartRF05 EB
 *   Processor    : Texas Instruments MSP430F2618
 *
 */

/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_mcu.h"
#include "hal_types.h"
#include "hal_defs.h"
#include "hal_board_cfg.h"


/* ------------------------------------------------------------------------------------------------
 *                                  Board Specific Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_VREG_SETTLE_TIME_USECS        50    /* microseconds */
#define HAL_MAC_RSSI_OFFSET                   -76   /* no units */
#if defined HAL_PA_LNA
#define HAL_MAC_RSSI_LNA_HGM_OFFSET           -8    /* dB */
#define HAL_MAC_RSSI_LNA_LGM_OFFSET           5     /* dB */
#elif defined HAL_PA_LNA_CC2590
#define HAL_MAC_RSSI_LNA_HGM_OFFSET           -6    /* dB */
#define HAL_MAC_RSSI_LNA_LGM_OFFSET           7     /* dB */
#endif

/* ------------------------------------------------------------------------------------------------
 *                                  FIFOP Pin / Interrupt Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_FIFOP_GPIO_BIT                  6
#define HAL_MAC_READ_FIFOP_PIN()                (P1IN & BV(HAL_MAC_FIFOP_GPIO_BIT))
#define HAL_MAC_CONFIG_FIFOP_PIN_AS_INPUT()     MAC_ASSERT(!(P1DIR & BV(HAL_MAC_FIFOP_GPIO_BIT)))

/* FIFOP interrupt macros */
#define HAL_MAC_FIFOP_INT_VECTOR()              PORT1_VECTOR
#define HAL_MAC_ENABLE_FIFOP_INT()              st( P1IE  |=  BV(HAL_MAC_FIFOP_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_DISABLE_FIFOP_INT()             st( P1IE  &= ~BV(HAL_MAC_FIFOP_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_CLEAR_FIFOP_INT_FLAG()          st( P1IFG &= ~BV(HAL_MAC_FIFOP_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_READ_FIFOP_INT_FLAG()           (P1IFG & BV(HAL_MAC_FIFOP_GPIO_BIT))
#define HAL_MAC_CONFIG_FIFOP_RISING_EDGE_INT()  st( P1IES &= ~BV(HAL_MAC_FIFOP_GPIO_BIT); ) /* atomic operation */


/* ------------------------------------------------------------------------------------------------
 *                                  SFD Pin / Interrupt Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_SFD_GPIO_BIT                    2
#define HAL_MAC_CONFIG_SFD_PIN_AS_INPUT()       st( P1DIR &= ~BV(HAL_MAC_SFD_GPIO_BIT); P1SEL |= BV(HAL_MAC_SFD_GPIO_BIT);)


/* SFD interrupt macros */
extern uint8 macMcuTimerProcessFallingEdgeSFDSync;
#define HAL_MAC_ENABLE_SFD_INT()                  st( macMcuTimerProcessFallingEdgeSFDSync = 1; )
#define HAL_MAC_DISABLE_SFD_INT()                 st( macMcuTimerProcessFallingEdgeSFDSync = 0; )
#define HAL_MAC_SFD_INT_IS_ENABLED()              ( macMcuTimerProcessFallingEdgeSFDSync )

#define HAL_MAC_CLEAR_SFD_INT_FLAG()   /* No action */


/* ------------------------------------------------------------------------------------------------
 *                                    FIFO Pin Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_FIFO_GPIO_BIT                   5
#define HAL_MAC_READ_FIFO_PIN()                 (P1IN & BV(HAL_MAC_FIFO_GPIO_BIT))
#define HAL_MAC_CONFIG_FIFO_PIN_AS_INPUT()      MAC_ASSERT(!(P1DIR & BV(HAL_MAC_FIFO_GPIO_BIT)))

/* FIFO interrupt macros */
#define HAL_MAC_FIFO_INT_VECTOR()               PORT1_VECTOR
#define HAL_MAC_ENABLE_FIFO_INT()               st( P1IE  |=  BV(HAL_MAC_FIFO_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_DISABLE_FIFO_INT()              st( P1IE  &= ~BV(HAL_MAC_FIFO_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_CLEAR_FIFO_INT_FLAG()           st( P1IFG &= ~BV(HAL_MAC_FIFO_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_READ_FIFO_INT_FLAG()            (P1IFG & BV(HAL_MAC_FIFO_GPIO_BIT))
#define HAL_MAC_CONFIG_FIFO_FALLING_EDGE_INT()  st( P1IES |= BV(HAL_MAC_FIFO_GPIO_BIT); ) /* atomic operation */


/* ------------------------------------------------------------------------------------------------
 *                                    CCA Pin Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_CCA_GPIO_BIT                    1
#define HAL_MAC_READ_CCA_PIN()                  (P1IN & BV(HAL_MAC_CCA_GPIO_BIT))
#define HAL_MAC_CONFIG_CCA_PIN_AS_INPUT()       st( P1DIR &= ~BV(HAL_MAC_CCA_GPIO_BIT); \
                                                    P1SEL &= ~BV(HAL_MAC_CCA_GPIO_BIT); \
                                                    CAPD &= ~BV(HAL_MAC_CCA_GPIO_BIT);  );

/* ------------------------------------------------------------------------------------------------
 *                            TX_ACK_DONE/TX_FRM_DONE Pin / Interrupt Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_TX_ACK_DONE_GPIO_BIT            3
#define HAL_MAC_READ_TX_ACK_DONE_PIN()          (P1IN & BV(HAL_MAC_TX_ACK_DONE_GPIO_BIT))
#define HAL_MAC_CONFIG_TX_ACK_DONE_PIN_AS_INPUT()   st( P1DIR &= ~BV(HAL_MAC_TX_ACK_DONE_GPIO_BIT); )
#define HAL_MAC_CONFIG_TX_ACK_DONE_PIN_AS_OUTPUT()  st( P1DIR |=  BV(HAL_MAC_TX_ACK_DONE_GPIO_BIT); )

/* TX_ACK_DONE interrupt macros (shared with TX_FRM_DONE) */
#define HAL_MAC_TX_ACK_DONE_INT_VECTOR()        PORT1_VECTOR
#define HAL_MAC_ENABLE_TX_ACK_DONE_INT()        st( P1IE  |=  BV(HAL_MAC_TX_ACK_DONE_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_DISABLE_TX_ACK_DONE_INT()       st( P1IE  &= ~BV(HAL_MAC_TX_ACK_DONE_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_TX_ACK_DONE_INT_IS_ENABLED()    (P1IE & BV(HAL_MAC_TX_ACK_DONE_GPIO_BIT))
#define HAL_MAC_CLEAR_TX_ACK_DONE_INT_FLAG()    st( P1IFG &= ~BV(HAL_MAC_TX_ACK_DONE_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_READ_TX_ACK_DONE_INT_FLAG()     (P1IFG & BV(HAL_MAC_TX_ACK_DONE_GPIO_BIT))
#define HAL_MAC_CONFIG_TX_ACK_DONE_RISING_EDGE_INT() st( P1IES &= ~BV(HAL_MAC_TX_ACK_DONE_GPIO_BIT); ) /* atomic operation */


/* ------------------------------------------------------------------------------------------------
 *                                 VREG_EN Pin Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_VREG_EN_GPIO_BIT                0
#define HAL_MAC_DRIVE_VREG_EN_PIN_HIGH()        st( P1OUT |=  BV(HAL_MAC_VREG_EN_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_DRIVE_VREG_EN_PIN_LOW()         st( P1OUT &= ~BV(HAL_MAC_VREG_EN_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_CONFIG_VREG_EN_PIN_AS_OUTPUT()  st( P1DIR |=  BV(HAL_MAC_VREG_EN_GPIO_BIT); )


/* ------------------------------------------------------------------------------------------------
 *                                  RESETN Pin Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_RESETN_GPIO_BIT                 7
#define HAL_MAC_DRIVE_RESETN_PIN_HIGH()         st( P5OUT |=  BV(HAL_MAC_RESETN_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_DRIVE_RESETN_PIN_LOW()          st( P5OUT &= ~BV(HAL_MAC_RESETN_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_CONFIG_RESETN_PIN_AS_OUTPUT()   st( P5DIR |=  BV(HAL_MAC_RESETN_GPIO_BIT); )


/* ------------------------------------------------------------------------------------------------
 *                                  TRIGGER Pin Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_TRIGGER_GPIO_BIT                7
#define HAL_MAC_CONFIG_TRIGGER_PIN_AS_OUTPUT()  st( P1DIR |=  BV(HAL_MAC_TRIGGER_GPIO_BIT); \
                                                    P1SEL |=  BV(HAL_MAC_TRIGGER_GPIO_BIT);)

#define HAL_MAC_CONFIG_TRIGGER_PIN_AS_INTPUT()  st( P1DIR &= ~BV(HAL_MAC_TRIGGER_GPIO_BIT); \
                                                    P1SEL &= ~BV(HAL_MAC_TRIGGER_GPIO_BIT);)

/* ------------------------------------------------------------------------------------------------
 *                                      SPI Configuration
 * ------------------------------------------------------------------------------------------------
 */

/* CSn Pin Configuration */
#define HAL_MAC_SPI_CSN_GPIO_BIT                  0
#define HAL_MAC_SPI_SET_CHIP_SELECT_ON()          st( P5OUT &= ~BV(HAL_MAC_SPI_CSN_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_SPI_SET_CHIP_SELECT_OFF()         st( P5OUT |=  BV(HAL_MAC_SPI_CSN_GPIO_BIT); ) /* atomic operation */
#define HAL_MAC_SPI_CHIP_SELECT_IS_OFF()          (P5OUT & BV(HAL_MAC_SPI_CSN_GPIO_BIT))
#define HAL_MAC_SPI_CONFIG_CSN_PIN_AS_OUTPUT()    st( P5DIR |= BV(HAL_MAC_SPI_CSN_GPIO_BIT); )

/* SCLK Pin Configuration */
#define HAL_MAC_SPI_CONFIG_SCLK_PIN_AS_OUTPUT()   st( P5DIR |= BV(3); P5SEL |= BV(3); ) /* also selects SPI pin functionality */

/* SI Pin Configuration */
#define HAL_MAC_SPI_CONFIG_SI_PIN_AS_OUTPUT()     st( P5DIR |= BV(1); P5SEL |= BV(1); ) /* also selects SPI pin functionality */

/* SO Pin Configuration */
#define HAL_MAC_SPI_CONFIG_SO_PIN_AS_INPUT()      st( MAC_ASSERT(!(P5DIR & BV(2)));  P5SEL |= BV(2); )  /* also selects SPI pin functionality */
#define HAL_MAC_SPI_READ_SO_PIN()                 (P5IN & BV(2))

/* read/write macros */
#define HAL_MAC_SPI_WRITE_BYTE(x)                 st( UC1IFG &= ~UCB1RXIFG;  UCB1TXBUF = x; )
#define HAL_MAC_SPI_READ_BYTE()                   UCB1RXBUF
#define HAL_MAC_SPI_WAIT_DONE()                   while(!(UC1IFG & UCB1RXIFG));

/* SPI critical section macros */
typedef halIntState_t halMacSpiIntState_t;
#define HAL_MAC_SPI_ENTER_CRITICAL_SECTION(x)     HAL_ENTER_CRITICAL_SECTION(x)
#define HAL_MAC_SPI_EXIT_CRITICAL_SECTION(x)      HAL_EXIT_CRITICAL_SECTION(x)

/*
 *  Chipcon CC2520 SPI Specifications
 * -----------------------------------------------
 *    Max SPI Clock   :  8 MHz
 *    Data Order      :  MSB transmitted first
 *    Clock Polarity  :  low when idle
 *    Clock Phase     :  sample leading edge
 */

/* initialization macro */
#define HAL_MAC_SPI_INIT() \
st ( \
  UCB1CTL1 = UCSSEL1 | UCSWRST;                 \
  UCB1CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC ;  \
  UCB1BR0  = 0;                                 \
  UCB1BR1  = 0;                                 \
  UCB1CTL1 &= ~UCSWRST;                         \
)

#define HAL_MAC_SPI_LUMINARY_SO_AS_GPIO()       /* Luminary workaround. No action */
#define HAL_MAC_SPI_LUMINARY_SO_RESTORE()       /* Luminary workaround. No action */
#define HAL_MAC_SPI_LUMINARY_READ_DUMMY_BYTE()  /* Luminary workaround. No action */


/* ------------------------------------------------------------------------------------------------
 *                                       Timer Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_TIMER_TICKS_PER_USEC                  HAL_CPU_CLOCK_MHZ

/* ---------------- Timer_A  --------------- */
#define HAL_MAC_TIMER_INIT()      st( TACCTL0 = 0; TACCTL1 = 0; TACCTL2 = 0;)

/* MCU chip bug when using mov instruction to modify TACTL
 * Make sure modifying TACTL results in bit set or clear assembly instead of
 * mov instruction. Otherwise use something like below...
 * st(asm("bis.w #0x210, &TACTL");)
 * st(asm("bic.w #0x10, &TACTL");)
 */
#define HAL_MAC_TIMER_START()     st( TACTL |= (TASSEL_2 | MC_1); )
#define HAL_MAC_TIMER_STOP()      st( TACTL &= ~(MC_1); )


/* ------------------------------------------------------------------------------------------------
 *                                  Rollover Macros
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_TIMER_SET_COUNT(x)                    st( TAR = x; )
#define HAL_MAC_TIMER_SET_ROLLOVER(x)                 st( TACCR0 = (x) - 1; )
#define HAL_MAC_TIMER_GET_ROLLOVER()                  ( TACCR0 )
#define HAL_MAC_TIMER_COUNT()                         TAR

#define HAL_MAC_TIMER_ROLLOVER_ISR_FUNCTION()         HAL_ISR_FUNCTION( macTimerRolloverIsr, TIMERA0_VECTOR )
#define HAL_MAC_TIMER_ROLLOVER_CLEAR_INTERRUPT()      st( TACCTL0 &= (CCIFG ^ 0xFFFF); ) /* atomic operation */
#define HAL_MAC_TIMER_ROLLOVER_ENABLE_INTERRUPT()     st( TACCTL0 |= CCIE; ) /* atomic operation */
#define HAL_MAC_TIMER_ROLLOVER_DISABLE_INTERRUPT()    st( TACCTL0 &= (CCIE ^ 0xFFFF); ) /* atomic operation */
#define HAL_MAC_TIMER_ROLLOVER_FLAG()                 (TACCTL0 & CCIFG)
#define HAL_MAC_TIMER_ROLLOVER_FORCE_INTERRUPT()      st( TACCTL0 |= CCIFG; ) /* atomic operation */

/* ------------------------------------------------------------------------------------------------
 *                                  Compare Macros
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_TIMER_CAPTURE_COMPARE_ISR_FUNCTION()  HAL_ISR_FUNCTION( halMacTimerCaptureCompareIsr, TIMERA1_VECTOR )

#define HAL_MAC_TIMER_COMPARE_ENABLE_INTERRUPT()      st( TACCTL2 |= CCIE; ) /* atomic operation */
#define HAL_MAC_TIMER_COMPARE_DISABLE_INTERRUPT()     st( TACCTL2 &= (CCIE ^ 0xFFFF); ) /* atomic operation */
#define HAL_MAC_TIMER_COMPARE_INTERRUPT_IS_ENABLED()  (TACCTL2 & CCIE)

#define HAL_MAC_TIMER_SET_COMPARE(x)                  st( TACCR2 = x; )
#define HAL_MAC_TIMER_CLEAR_COMPARE_FLAG()            st( TACCTL2 &= (CCIFG ^ 0xFFFF); ) /* atomic operation */
#define HAL_MAC_TIMER_COMPARE_FLAG()                  (TACCTL2 & CCIFG)

#define HAL_MAC_TIMER_ENABLE_OUTPUT_COMPARE()         st( TACCTL2 |= OUTMOD_1;) /* out mode = SET i.e. out = 1 when tar == taccr2 */
#define HAL_MAC_TIMER_DISABLE_OUTPUT_COMPARE()        st( TACCTL2 =  0x0;) /* sets out value = 0 and disables the interrupt */

/* ------------------------------------------------------------------------------------------------
 *                                  Capture Macros
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_TIMER_CONFIGURE_INPUT_CAPTURE()       st( TACCTL1 =  0x0;     /* Clear the previous settings. */ \
                                                          TACCTL1 |= (CCIS_0 | CAP | SCS | CM_3 | CCIE);)
/* Select CCI-1A |  Capture Mode  | Synchronous capture | Capture on both edges | Enable Interrupt on capture */

#define HAL_MAC_TIMER_CAPTURED_INPUT_IS_HIGH()           (TACCTL1 & CCI)
#define HAL_MAC_TIMER_SFD_CAPTURE()                      (TACCR1)

/* ------------------------------------------------------------------------------------------------
 *                                  Timer Interrupt Vectio Register
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MAC_TAIV()                                  ( TAIV )

/* ------------------------------------------------------------------------------------------------
 *                                  Power Management Macros
 * ------------------------------------------------------------------------------------------------
 */
/* MSP430 clock control register mask */
#define MSP430_CLK_CTRL_MASK                            ( MC_0|MC_1|MC_2|MC_3|ID_0|ID_1|ID_2|ID_3|TASSEL_0|TASSEL_1|TASSEL_2|TASSEL_3 )
#define HAL_MAC_SLEEP_TIMER_TAR()                       ( TAR )
#define HAL_MAC_SLEEP_TIMER_COMPARE()                   ( TACCR0 )
#define HAL_MAC_SLEEP_TIMER_SET_COMPARE(x)            st( TACCR0 = x; )
#define HAL_MAC_SLEEP_TIMER_SLOW_DOWN()               st( TACTL &= ~MSP430_CLK_CTRL_MASK; \
                                                          TACTL |= (MC_0 | ID_3 | TASSEL_1); )
#define HAL_MAC_SLEEP_TIMER_SPEED_UP()                st( TACTL &= ~MSP430_CLK_CTRL_MASK; \
                                                          TACTL |= (MC_0 | ID_0 | TASSEL_2 ); )
#define HAL_MAC_SLEEP_TIMER_HALT()                    st( TACTL &= ~(MC_0|MC_1|MC_2|MC_3); )
#define HAL_MAC_SLEEP_TIMER_RESTART()                 st( TACTL &= ~MC_0; \
                                                          TACTL |=  MC_1; )


/* ------------------------------------------------------------------------------------------------
 *                                       Prototypes
 * ------------------------------------------------------------------------------------------------
 */
void halMacInit(void);

/**************************************************************************************************
*/
#endif
