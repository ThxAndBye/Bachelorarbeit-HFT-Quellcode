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

-------------------------- END-OF-HEADER -----------------------------
File    : OS_StartLEDBlink.c
Purpose : embOS sample program running two simple tasks, each toggling
          an LED of the target hardware (as configured in BSP.c).
*/

#include "BSP.h"
#include "RTOS.h"
#include "SEGGER_SYSVIEW_embOS.h"
#include <stdio.h>
#include <stdint.h>

static OS_STACKPTR int StackHP[128], StackLP[128]; // Task stacks
static OS_TASK TCBHP, TCBLP;                       // Task control blocks

static void HPTask(void) {
  while (1) {
    BSP_ToggleLED(0);
    OS_TASK_Delay(50);
  }
}

static uint8_t isPrime(uint32_t zahl) {
  for (uint32_t teiler = 2; teiler < zahl / 2; teiler++) {
    if (zahl % teiler == 0)
      return 0;
  }
  return 1;
}

static void LPTask(void) {
  uint32_t zahl = 2;
  while (1) {
    if (isPrime(zahl))
      printf("%ld\n", zahl);
    zahl++;
  }
}

/*********************************************************************
*
*       main()
*/
int main(void) {
  OS_Init();              // Initialize embOS
  OS_InitHW();            // Initialize required hardware
  BSP_Init();             // Initialize LED port
  SEGGER_SYSVIEW_Conf();  // Configure and initialize SystemView
  OS_TASK_CREATE(&TCBHP, "HP Task", 100, HPTask, StackHP);
  OS_TASK_CREATE(&TCBLP, "LP Task", 50, LPTask, StackLP);
  OS_Start();             // Start embOS
  return 0;
}

/*************************** End of file ****************************/