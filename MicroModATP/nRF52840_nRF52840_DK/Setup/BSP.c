/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2021 SEGGER Microcontroller GmbH                  *
*                                                                    *
*       Internet: segger.com  Support: support_embos@segger.com      *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS * Real time operating system for microcontrollers      *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product or a real-time            *
*       operating system for in-house use.                           *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       OS version: V5.14.0.0                                        *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : BSP.c
Purpose : BSP for SparkFun MicroMod nRF52840 Processor
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "BSP.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define LED1               (0u)

#define MAX_LEDS           (1u)

#define LED1_PIN           (13u)  // STAT LED P0.13

#define LED_MASK(x)        (1u << (13u + ((x) & 0x3u)))

#define GPIO0_BASE_ADDR    (0x50000000u)
#define GPIO0_OUT          (* (volatile unsigned int*)(GPIO0_BASE_ADDR + 0x504u))
#define GPIO0_OUTSET       (* (volatile unsigned int*)(GPIO0_BASE_ADDR + 0x508u))
#define GPIO0_OUTCLR       (* (volatile unsigned int*)(GPIO0_BASE_ADDR + 0x50Cu))
#define GPIO0_PIN_CNF(x)   (*((volatile unsigned int*)(GPIO0_BASE_ADDR + 0x700u) + (x)))

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       BSP_Init()
*/
void BSP_Init(void) {
  GPIO0_OUTSET = LED_MASK(0);  // Turn off STAT LED
  GPIO0_PIN_CNF(LED1_PIN) = 0
                          | (0x1u <<  0u)  // Set LED as output
                          | (0x1u <<  1u)  // Disonnect input buffer
                          | (0x0u <<  2u)  // No pull
                          | (0x0u <<  8u)  // Standard '0', standard '1'
                          | (0x0u << 16u)  // Disabled pin sensing mechanism
                          ;
}

/*********************************************************************
*
*       BSP_SetLED()
*/
void BSP_SetLED(int Index) {
  if (((unsigned int)Index) < MAX_LEDS) {
    GPIO0_OUTCLR = LED_MASK(Index);
  }
}

/*********************************************************************
*
*       BSP_ClrLED()
*/
void BSP_ClrLED(int Index) {
  if (((unsigned int)Index) < MAX_LEDS) {
    GPIO0_OUTSET = LED_MASK(Index);
  }
}

/*********************************************************************
*
*       BSP_ToggleLED()
*/
void BSP_ToggleLED(int Index) {
  if (((unsigned int)Index) < MAX_LEDS) {
    if (GPIO0_OUT & LED_MASK(Index)) {
      GPIO0_OUTCLR = LED_MASK(Index);
    } else {
      GPIO0_OUTSET = LED_MASK(Index);
    }
  }
}

/****** End Of File *************************************************/
