/**************************************************************************************************
  Filename:       mac_radio_defs.c
  Revised:        $Date: 2007-09-11 18:59:21 -0700 (Tue, 11 Sep 2007) $
  Revision:       $Revision: 15378 $

  Description:    Describe the purpose and contents of the file.


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

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

/* ------------------------------------------------------------------------------------------------
 *                                             Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "mac_radio_defs.h"
#include "hal_types.h"
#include "hal_assert.h"


/* ------------------------------------------------------------------------------------------------
 *                                        Global Constants
 * ------------------------------------------------------------------------------------------------
 */
const uint8 CODE macRadioDefsTxPowerTable[] =
{
  /*   0 dBm */   0xFF,   /* characterized value from datasheet */
  /*  -1 dBm */   0xFB,   /* characterized value from datasheet */
  /*  -2 dBm */   0xFB,
  /*  -3 dBm */   0xFB,   /* characterized value from datasheet */
  /*  -4 dBm */   0xF7,
  /*  -5 dBm */   0xF3,   /* characterized value from datasheet */
  /*  -6 dBm */   0xF3,
  /*  -7 dBm */   0xEF,   /* characterized value from datasheet */
  /*  -8 dBm */   0xEF,
  /*  -9 dBm */   0xEF,
  /* -10 dBm */   0xEB,   /* characterized value from datasheet */
  /* -11 dBm */   0xEB,
  /* -12 dBm */   0xEB,
  /* -13 dBm */   0xEB,
  /* -14 dBm */   0xEB,
  /* -15 dBm */   0xE7,   /* characterized value from datasheet */
  /* -16 dBm */   0xE7,
  /* -17 dBm */   0xE7,
  /* -18 dBm */   0xE7,
  /* -19 dBm */   0xE7,
  /* -20 dBm */   0xE7,
  /* -21 dBm */   0xE7,
  /* -22 dBm */   0xE7,
  /* -23 dBm */   0xE7,
  /* -24 dBm */   0xE7,
  /* -25 dBm */   0xE3    /* characterized value from datasheet */
};


/**************************************************************************************************
 *                                  Compile Time Integrity Checks
 **************************************************************************************************
 */
HAL_ASSERT_SIZE(macRadioDefsTxPowerTable, MAC_RADIO_TX_POWER_MAX_MINUS_DBM+1);  /* array size mismatch */


/**************************************************************************************************
 */


void macTxFrame()
{
	% radioinit
	% set tx power
	% set channel
    /*
     *  Flush the TX FIFO.  This is necessary in case the previous transmit was never
     *  actually sent (e.g. CSMA failed without strobing TXON).  If bytes are written to
     *  the FIFO but not transmitted, they remain in the FIFO to be transmitted whenever
     *  a strobe of TXON does happen.
     */
    MAC_RADIO_FLUSH_TX_FIFO();

    /* write bytes to FIFO, prepended byte is included, MFR is not (it's generated by hardware) */
    MAC_RADIO_WRITE_TX_FIFO(p, PREPENDED_BYTE_LEN + lenMhrMsdu);
	
}

/**************************************************************************************************
 * @fn          macRxHardDisable
 *
 * @brief       Clear all enable flags and turn off receiver.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void macRxHardDisable(void)
{
  halIntState_t  s;

  HAL_ENTER_CRITICAL_SECTION(s);

  macRxEnableFlags = 0;
  macRxOnFlag = 0;

  /* force receiver off */
  MAC_RADIO_RXTX_OFF();
  MAC_RADIO_FLUSH_RX_FIFO();
  MAC_DEBUG_TURN_OFF_RX_LED();

  HAL_EXIT_CRITICAL_SECTION(s);

  /* clean up after being forced off */
  macRxHaltCleanup();
}

/**************************************************************************************************
 * @fn          macRxOn
 *
 * @brief       Turn on the receiver if it's not already on.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void macRxOn(void)
{
  halIntState_t  s;

  HAL_ENTER_CRITICAL_SECTION(s);
  if (!macRxOnFlag)
  {
    macRxOnFlag = 1;
    MAC_RADIO_RX_ON();
    MAC_DEBUG_TURN_ON_RX_LED();
  }
  HAL_EXIT_CRITICAL_SECTION(s);
}


/**************************************************************************************************
 * @fn          macRxOff
 *
 * @brief       Turn off the receiver if it's not already off.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void macRxOff(void)
{
  halIntState_t  s;

  HAL_ENTER_CRITICAL_SECTION(s);
  if (macRxOnFlag)
  {
    macRxOnFlag = 0;
    MAC_RADIO_RXTX_OFF();
    MAC_DEBUG_TURN_OFF_RX_LED();
    
    /* just in case a receive was about to start, flush the receive FIFO */
    MAC_RADIO_FLUSH_RX_FIFO();

    /* clear any receive interrupt that happened to squeak through */
    MAC_RADIO_CLEAR_RX_THRESHOLD_INTERRUPT_FLAG();

  }
  HAL_EXIT_CRITICAL_SECTION(s);
}



/**************************************************************************************************
 * @fn          macLowLevelInit
 *
 * @brief       Initialize low-level MAC.  Called only once on system power-up.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void macLowLevelInit(void)
{
  /* initialize mcu before anything else */
  MAC_RADIO_MCU_INIT();

  /* software initialziation */
  macRadioInit();
  macRxOnOffInit();
  macRxInit();
  macTxInit();
  macBackoffTimerInit();
}


/**************************************************************************************************
 * @fn          macLowLevelReset
 *
 * @brief       Reset low-level MAC.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void macLowLevelReset(void)
{
  MAC_ASSERT(!HAL_INTERRUPTS_ARE_ENABLED());   /* interrupts must be disabled */

  /* reset radio modules;  if not awake, skip this */
  if (macSleepState == MAC_SLEEP_STATE_AWAKE)
  {
    macRxTxReset();
    macRadioReset();
  }

  /* reset timer */
  macBackoffTimerReset();

  /* power up the radio */
  macSleepWakeUp();
}


/* ------------------------------------------------------------------------------------------------
 *                                         Global Variables
 * ------------------------------------------------------------------------------------------------
 */
uint8 macSleepState = MAC_SLEEP_STATE_RADIO_OFF;


/**************************************************************************************************
 * @fn          macSleepWakeUp
 *
 * @brief       Wake up the radio from sleep mode.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void macSleepWakeUp(void)
{
  /* don't wake up radio if it's already awake */
  if (macSleepState == MAC_SLEEP_STATE_AWAKE)
  {
    return;
  }

  /* wake up MAC timer */
  MAC_RADIO_TIMER_WAKE_UP();

  /* if radio was completely off, restore from that state first */
  if (macSleepState == MAC_SLEEP_STATE_RADIO_OFF)
  {
    /* turn on radio power */
    MAC_RADIO_TURN_ON_POWER();
    
    /* power-up initialization of receive logic */
    macRxRadioPowerUpInit();
  }

  /* turn on the oscillator */
  MAC_RADIO_TURN_ON_OSC();
  
  /* update sleep state here before requesting to turn on receiver */
  macSleepState = MAC_SLEEP_STATE_AWAKE;

  /* turn on the receiver if enabled */
  macRxOnRequest();
}


/**************************************************************************************************
 * @fn          macSleep
 *
 * @brief       Puts radio into the selected sleep mode.
 *
 * @param       sleepState - selected sleep level, see #defines in .h file
 *
 * @return      TRUE if radio was successfully put into selected sleep mode.
 *              FALSE if it was not safe for radio to go to sleep.
 **************************************************************************************************
 */
uint8 macSleep(uint8 sleepState)
{
  halIntState_t  s;
 
  /* disable interrupts until macSleepState can be set */
  HAL_ENTER_CRITICAL_SECTION(s);

  /* assert checks */
  MAC_ASSERT(macSleepState == MAC_SLEEP_STATE_AWAKE); /* radio must be awake to put it to sleep */
  MAC_ASSERT(macRxFilter == RX_FILTER_OFF); /* do not sleep when scanning or in promiscuous mode */

  /* if either RX or TX is active or any RX enable flags are set, it's not OK to sleep */
  if (macRxActive || macRxOutgoingAckFlag || macTxActive || macRxEnableFlags)
  {
    HAL_EXIT_CRITICAL_SECTION(s);
    return(FALSE);
  }

  /* turn off the receiver */
  macRxOff();

  /* update sleep state variable */
  macSleepState = sleepState;

  /* macSleepState is now set, re-enable interrupts */
  HAL_EXIT_CRITICAL_SECTION(s);
  
  /* put MAC timer to sleep */
  MAC_RADIO_TIMER_SLEEP();

  /* put radio in selected sleep mode */
  if (sleepState == MAC_SLEEP_STATE_OSC_OFF)
  {
    MAC_RADIO_TURN_OFF_OSC();
  }
  else
  {
    MAC_ASSERT(sleepState == MAC_SLEEP_STATE_RADIO_OFF); /* unknown sleep state */
    MAC_RADIO_TURN_OFF_POWER();
  }

  /* radio successfully entered sleep mode */
  return(TRUE);
}

