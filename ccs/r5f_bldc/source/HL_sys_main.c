/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 07-July-2017
*   @version 04.07.00
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
#include "HL_gio.h"
#include "HL_sci.h"
#include "HL_etpwm.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
void wait(uint32 delay);
void disp_set(char *str);
void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 len);
uint32 recieve_data(void);
void put_pwm(void);
void calculate_persent(uint16 data);
void calv_bldc(void);

volatile uint16 pwm_data =0;
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    sciInit();
    gioInit();
    etpwmInit();
    etpwmStartTBCLK();
    calv_bldc();
    disp_set("finish initialize");

    for(;;)
    {
        put_pwm();
        wait(100);
        calculate_persent(pwm_data);
    }
/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */

void wait(uint32 delay)
{
    int i;

    for (i = 0; i < delay; i++)
        ;
}

uint32 recieve_data(void)
{
    uint8 tmp[8]={0,};
    sciReceive(sciREG1,6,tmp);
    return (tmp[0]-'0')*1000+(tmp[1]-'0')*100+(tmp[2]-'0')*10+(tmp[3]-'0');
}

void disp_set(char *str)
{
    char txt_buf[256] = {0};
    unsigned int buf_len;
    sprintf(txt_buf, str);
    buf_len = strlen(txt_buf);
    sciDisplayText(sciREG1, (uint8 *)txt_buf, buf_len);
    wait(100000);
}

void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 len)
{
    while(len--)
    {
        while((sciREG1->FLR & 0x4) == 4)
            ;
        sciSendByte(sciREG1, *text++);
    }
}

void put_pwm(void)
{
    pwm_data= (uint16)(recieve_data()*5/2);
    etpwmSetCmpA(etpwmREG1,pwm_data);
}

void calv_bldc(void)
{
    etpwmSetCmpA(etpwmREG1,2500);
    wait(1000000);
    etpwmSetCmpA(etpwmREG1,5000);
    wait(1000000);
    etpwmSetCmpA(etpwmREG1,2500);
}

void calculate_persent(uint16 data)
{
    char tmp[128] = {0,};
    float persent =0;
    unsigned int buf_len;

    persent = (float)pwm_data/500000.0;
    sprintf(tmp,"pwm persent = %f\n\r",persent);
    buf_len = strlen(tmp);

    sciDisplayText(sciREG1, (uint8 *)tmp, buf_len);
    wait(100000);
}
/* USER CODE END */
