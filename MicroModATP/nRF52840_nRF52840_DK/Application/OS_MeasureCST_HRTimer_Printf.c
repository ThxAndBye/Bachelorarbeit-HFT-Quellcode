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
File    : OS_MeasureCST_HRTimer_Printf.c
Purpose : embOS sample program that measures the embOS context
          switching time and displays the result on a terminal I/O
          window. It runs with every workbench that supports terminal
          I/O with printf.
*/

#include "RTOS.h"
#include <stdio.h>

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static OS_STACKPTR int StackHP[128], StackLP[128];  // Task stacks
static OS_TASK         TCBHP, TCBLP;                // Task control blocks
static OS_U32          Time;

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       HPTask()
*/
static void HPTask(void) {
  while (1) {
    OS_TASK_Suspend(NULL);           // Suspend high priority task
    OS_TIME_StopMeasurement(&Time);  // Stop measurement
  }
}

/*********************************************************************
*
*       LPTask()
*/
static void LPTask(void) {
  OS_U32 MeasureOverhead;  // Time for Measure Overhead
  OS_U32 v;                // Real context switching time

  while (1) {
    //
    // Measure overhead for time measurement so we can take this into account by subtracting it
    // This is done inside the while()-loop to mitigate possible effects of an instruction cache
    //
    OS_TIME_StartMeasurement(&MeasureOverhead);
    OS_TIME_StopMeasurement(&MeasureOverhead);
    //
    // Perform actual measurements
    //
    OS_TASK_Delay(100);                        // Synchronize to tick to avoid jitter
    OS_TIME_StartMeasurement(&Time);           // Start measurement
    OS_TASK_Resume(&TCBHP);                    // Resume high priority task to force task switch
    v  = OS_TIME_GetResult(&Time);
    v -= OS_TIME_GetResult(&MeasureOverhead);  // Calculate real context switching time (w/o measurement overhead)
    v  = OS_TIME_ConvertCycles2ns(v);          // Convert cycles to nano-seconds
    printf("Context switch time: %lu.%.3lu usec\r\n", (v / 1000uL), (v % 1000uL));  // Print out result
  }
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       main()
*/
int main(void) {
  OS_Init();    // Initialize embOS
  OS_InitHW();  // Initialize required hardware
  OS_TASK_CREATE(&TCBHP, "HP Task", 100, HPTask, StackHP);
  OS_TASK_CREATE(&TCBLP, "LP Task",  50, LPTask, StackLP);
  OS_Start();   // Start embOS
  return 0;
}

/*************************** End of file ****************************/
