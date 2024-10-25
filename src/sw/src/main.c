/****************************************************************************
 * RFsoc-dfe Main
 *
 *
 ***************************************************************************/

#include <stdio.h>
#include <time.h>
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

#include "xparameters.h"
#include "xrfdc.h"



XIicPs IicPsInstance;	    // Instance of the IIC Device
XRFdc RFdcInst;  // RFDC instance



int main()
{
	u32 i=0;
	u32 j=0;
	time_t epoch_time;
	struct tm *human_time;
	char timebuf[80];

    init_platform();


    xil_printf("Hello rfSOC World...\n\r");
    xil_printf("Module ID Number: %x\r\n", Xil_In32(XPAR_M_AXI_BASEADDR + 0x0));
    xil_printf("Module Version Number: %x\r\n", Xil_In32(XPAR_M_AXI_BASEADDR + 0x4));
    xil_printf("Project ID Number: %x\r\n", Xil_In32(XPAR_M_AXI_BASEADDR + 0x10));
    xil_printf("Project Version Number: %x\r\n", Xil_In32(XPAR_M_AXI_BASEADDR + 0x14));
    //compare to git commit with command: git rev-parse --short HEAD
    xil_printf("Git Checksum: %x\r\n", Xil_In32(XPAR_M_AXI_BASEADDR + 0x18));
    epoch_time = Xil_In32(XPAR_M_AXI_BASEADDR + 0x1C);
    human_time = localtime(&epoch_time);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", human_time);
    xil_printf("Project Compilation Timestamp: %s\r\n", timebuf);



    init_i2c();

    // Read on-board temperature sensors
    read_board_temps();

    //read voltage & currents from LTC2991 chips
  	i2c_configure_ltc2991();
    i2c_get_ltc2991();

    // Program the CLK104 PLL
    WriteLMK04828();
    sleep(1);

    // Initialize the RFdc subsystem
    InitRFdc();
    sleep(2);

    i2c_get_ltc2991();

 
    //Reset & Trigger the ADC0 FIFO
    u32 wdcnt;
    Xil_Out32(XPAR_M_AXI_BASEADDR + 0x48, 1);
    sleep(1);
    wdcnt = Xil_In32(XPAR_M_AXI_BASEADDR + 0x44);
    xil_printf("FIFO Wdcnt = %d\r\n",wdcnt);
    //Trigger
    Xil_Out32(XPAR_M_AXI_BASEADDR + 0x4C, 1);
    sleep(1);
    wdcnt = Xil_In32(XPAR_M_AXI_BASEADDR + 0x44);
    xil_printf("FIFO Wdcnt = %d\r\n",wdcnt);

    u32 data;
    s16 adcval;
    //read out the fifo
    for (i=0;i<100;i++) {
    	for (j=0;j<8;j++) {
    	    if (j<6) {	
    	        data = Xil_In32(XPAR_M_AXI_BASEADDR + 0x40); 
   	            adcval = (s16) ((data & 0xFFFF0000) >> 16);
   	            xil_printf("%d\r\n",adcval>>2);
   	            adcval = (s16) (data & 0xFFFF);       	
                xil_printf("%d\r\n",adcval>>2);
    	    }
            else
       	        data = Xil_In32(XPAR_M_AXI_BASEADDR + 0x40); 
        }
    }
     
    wdcnt = Xil_In32(XPAR_M_AXI_BASEADDR + 0x44);
     xil_printf("FIFO Wdcnt = %d\r\n",wdcnt);    

    
    
    //blink some FP leds
    while (1) {
    	Xil_Out32(XPAR_M_AXI_BASEADDR + 0x20, i++);
    	sleep(0.1);
    	Xil_In32(XPAR_M_AXI_BASEADDR + 0x44);
    	//fpgabase[8] = i++;
    	xil_printf("%d:  %d\r\n",i,Xil_In32(XPAR_M_AXI_BASEADDR + 0x20)); //fpgabase[8]);
        sleep(1);
    }

    print("Successfully ran Hello World application");
    cleanup_platform();
    return 0;
}

