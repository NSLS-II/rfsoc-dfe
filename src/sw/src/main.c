/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xiicps.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "lwip/init.h"

#include "rfsoc-dfe_defs.h"


XIicPs IicPsInstance;	    // Instance of the IIC Device





int main()
{
	int i;
	float temp;
	unsigned int *fpgabase;
    init_platform();

    print("Hello World\n\r");


    init_i2c();

    //read temperature sensors
    i2c_set_port_expander(I2C_PORTEXP1_ADDR,1);
    temp = read_i2c_temp(BRDTEMP0_ADDR);
    printf("Board Temp0: %f\r\n", temp);
    temp = read_i2c_temp(BRDTEMP1_ADDR);
    printf("Board Temp1: %f\r\n", temp);
    temp = read_i2c_temp(BRDTEMP2_ADDR);
    printf("Board Temp2: %f\r\n", temp);
    temp = read_i2c_temp(BRDTEMP3_ADDR);
    printf("Board Temp3: %f\r\n", temp);
    
    //read voltage & currents from LTC2991 chips
    for (i=0;i<5;i++) {
 	    i2c_set_port_expander(I2C_PORTEXP1_ADDR,4);
 	    i2c_configure_ltc2991();
        i2c_get_ltc2991();
        sleep(1);
    }

    fpgabase = (unsigned int *)0xA0000000;

    //blink some FP leds
    for (i=0;i<20;i++) {
    	fpgabase[8] = i;
    	xil_printf("%d:  %d\r\n",i,fpgabase[8]);
        sleep(1);
    }

    print("Successfully ran Hello World application");
    cleanup_platform();
    return 0;
}
