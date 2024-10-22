#include "xparameters.h"
#include "xiicps.h"
#include "rfsoc-dfe_defs.h"
#include <sleep.h>
#include "xil_printf.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "xrfdc.h"

#include "rfsoc-dfe_defs.h"

extern XRFdc RFdcInst;  // RFDC instance


/*****************************************************************************/
/**
*
* My libmetal logger
* Intercepts log prints and adjusts \r\n prints to display the some on a uart
* or through a jtagUart.
*
******************************************************************************/

void my_metal_default_log_handler(enum metal_log_level level,
			       const char *format, ...)
{
	char msg[1024];
	char msgOut[1048];
	char *outPtr;
	int i;

	va_list args;
	static const char *level_strs[] = {
		"metal: emergency: ",
		"metal: alert:     ",
		"metal: critical:  ",
		"metal: error:     ",
		"metal: warning:   ",
		"metal: notice:    ",
		"metal: info:      ",
		"metal: debug:     ",
	};

	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);
	va_end(args);

	//replace single \n with \n\r
	outPtr = msgOut;
	for(i=0; i<1024; i++) {
		// if /n/r or /r/n combo
		if ((msg[i] == '\r' && msg[i+1] == '\n') ||
				(msg[i] == '\n' && msg[i+1] == '\r')) {
			*outPtr++ = msg[i++];
		} else if(msg[i] == '\n') {
			//if first char in string is \n, then remove
			if(i==0) {
				continue;
			} else {
				*outPtr++ = '\r';
			}
		}
		*outPtr++ = msg[i];
		if(msg[i] == 0) {
			break;
		}
	}
	//if line doesn't end with \n\r, then add it
	if( (msg[i-1] != '\n') && (msg[i-1] != '\r') ) {
		*(outPtr-1) = '\r';
		*outPtr++ = '\n';
		*outPtr++ = 0;
	}

	if (level <= METAL_LOG_EMERGENCY || level > METAL_LOG_DEBUG)
		level = METAL_LOG_EMERGENCY;

	xil_printf("%s%s", level_strs[level], msgOut);
}


u32 InitRFdc()
{
	u32 Tile_Id;
	u32 val;
	XRFdc_IPStatus ipStatus;
	XRFdc* RFdcInstPtr = &RFdcInst;
	int Status;

    XRFdc_Config *ConfigPtr;



	 metal_set_log_handler(my_metal_default_log_handler);
	 metal_set_log_level(METAL_LOG_DEBUG);

    //xil_printf("XRFdc Reset\r\n");
    //Xil_Out32(XPAR_XRFDC_0_BASEADDR + 4, 1);
    //sleep(1);




    xil_printf("XRFdc LookupConfig...\r\n");
    // Step 1: Initialize RFDC driver
    ConfigPtr = XRFdc_LookupConfig(XPAR_XRFDC_0_DEVICE_ID);
    if (ConfigPtr == NULL) {
        printf("RFDC configuration lookup failed!\n");
        return XST_FAILURE;
    }


    xil_printf("XRFdc CfgInitialize...\r\n");
    Status = XRFdc_CfgInitialize(&RFdcInst, ConfigPtr);
    if (Status != XST_SUCCESS) {
        printf("RFDC initialization failed!\n");
        return XST_FAILURE;
    }


	// Display IP version
	xil_printf("Version : 0X%X\r\n", Xil_In32 (XPAR_XRFDC_0_BASEADDR + 0) );

	xil_printf("ADC0 Current State : %x\r\n", Xil_In32 (XPAR_XRFDC_0_BASEADDR + 0x14000 + 0xC));
	xil_printf("ADC1 Current State : %x\r\n", Xil_In32 (XPAR_XRFDC_0_BASEADDR + 0x18000 + 0xC));
	xil_printf("ADC2 Current State : %x\r\n", Xil_In32 (XPAR_XRFDC_0_BASEADDR + 0x1C000 + 0xC));
	xil_printf("ADC3 Current State : %x\r\n", Xil_In32 (XPAR_XRFDC_0_BASEADDR + 0x20000 + 0xC));

	xil_printf("ADC0 Clock Detect : %x\r\n", Xil_In32 (XPAR_XRFDC_0_BASEADDR + 0x14000 + 0x84));
	xil_printf("ADC1 Clock Detect : %x\r\n", Xil_In32 (XPAR_XRFDC_0_BASEADDR + 0x18000 + 0x84));
	xil_printf("ADC2 Clock Detect : %x\r\n", Xil_In32 (XPAR_XRFDC_0_BASEADDR + 0x1C000 + 0x84));
	xil_printf("ADC3 Clock Detect : %x\r\n", Xil_In32 (XPAR_XRFDC_0_BASEADDR + 0x20000 + 0x84));



 	XRFdc_GetIPStatus(&RFdcInst, &ipStatus);


 	// startup
 	for ( Tile_Id=0; Tile_Id<=3; Tile_Id++) {
 		if (ipStatus.DACTileStatus[Tile_Id].IsEnabled == 1) {
 			val = XRFdc_ReadReg16(RFdcInstPtr, XRFDC_ADC_TILE_CTRL_STATS_ADDR(Tile_Id), XRFDC_ADC_DEBUG_RST_OFFSET);
 			if(val & XRFDC_DBG_RST_CAL_MASK) {
 				xil_printf("  Tile: %d NOT ready.\r\n", Tile_Id);
 			} else {
 				XRFdc_StartUp(&RFdcInst, 1, Tile_Id);
 				usleep(200000);
 			}
 		}
 	}



 	for ( Tile_Id=0; Tile_Id<=3; Tile_Id++) {
 		if (ipStatus.ADCTileStatus[Tile_Id].IsEnabled == 1) {
 			val = XRFdc_ReadReg16(RFdcInstPtr, XRFDC_ADC_TILE_CTRL_STATS_ADDR(Tile_Id), XRFDC_ADC_DEBUG_RST_OFFSET);
 			if(val & XRFDC_DBG_RST_CAL_MASK) {
 				xil_printf("  ADC Tile%d NOT ready.\r\n", Tile_Id);
 			} else {
 				XRFdc_StartUp(&RFdcInst, 0, Tile_Id);
 				usleep(200000);
 			}
 		}
 	}

return 0;

}


