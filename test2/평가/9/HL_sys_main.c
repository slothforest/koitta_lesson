/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 08-Feb-2017
*   @version 04.06.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
#include "HL_sci.h"
#include "HL_gio.h"
#include "HL_system.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
void task0_func(void *pbParameters);
void task1_func(void *pbParameters);
void wait(uint32 delay);
void recieve_data(uint8 *data);
void sci_display(sciBASE_t *sci, uint8 *text,  uint32 len);

xTaskHandle task0;
xTaskHandle task1;

uint8 rvstring[8] ={0,};
uint8 dsstring[256]={0,};
uint32 led_data =0;
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    sciInit();

    gioInit();
    gioSetDirection(gioPORTA,0x0001<<4);


    if(xTaskCreate(task0_func,"decision",
                   configMINIMAL_STACK_SIZE,NULL,5 /*우선순위 숫자 큰게 */,&task0) != pdTRUE)
    {
        for(;;)
              ;
    }
    if(xTaskCreate(task1_func,"collision_detect",
                   configMINIMAL_STACK_SIZE,NULL,1/*우선순위  숫자큰게 */,&task1) != pdTRUE)
    {
        for(;;)
              ;
    }
/* USER CODE END */

    return 0;
}

/* USER CODE BEGIN (4) */
void task0_func(void *pbParameters)
{
    for(;;)
    {
        gioSetBit(gioPORTA,4,led_data^1);
        wait(333333);
    }
}

void task1_func(void *pbParameters)
{
    for(;;)
    {
        recieve_data(rvstring);
        if(!strcmp((char *)rvstring,"DONG\r\n"))
         {
            sprintf((char *)dsstring,"hello\r\n");
            sci_display(sciREG1,dsstring,7);
         }
    }
}

void wait(uint32 delay)
{
    int i;
    for(i=0;i<delay;i++)
        ;
}

void recieve_data(uint8 *data)
{
    sciReceive(sciREG1,6,data);
}

void sci_display(sciBASE_t *sci, uint8 *text,  uint32 len)
{
    while(len--)
    {
#if 0
        while((UART->FLR & 0x4)==4)
            ;
#endif
        sciSendByte(sci,*text++);
    }
}
/* USER CODE END */
