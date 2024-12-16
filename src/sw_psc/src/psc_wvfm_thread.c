/********************************************************************
*  PSC Waveform Thread
*  J. Mead
*  4-17-24
*
*  This thread is responsible for sending all waveform data to the IOC.   It does
*  this over to message ID's (51 = ADC Data, 52 = TbT data)
*
*  It starts a listening server on
*  port 600.  Upon establishing a connection with a client, it begins to send out
*  packets.
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <sleep.h>
#include "xil_cache.h"

#include "lwip/sockets.h"
#include "netif/xadapter.h"
#include "lwipopts.h"
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"

/* Hardware support includes */
#include "rfsoc-dfe_defs.h"
#include "pl_regs.h"
#include "psc_msg.h"


#define PORT  20



void soft_trig() {
	Xil_Out32(XPAR_M_AXI_BASEADDR + ADCFIFO_TRIG, 1);
	usleep(1000);
	Xil_Out32(XPAR_M_AXI_BASEADDR + ADCFIFO_TRIG, 0);
}


void Host2NetworkConvWvfm(char *inbuf, int len) {

    int i;
    u8 temp;
    //Swap bytes to reverse the order within the 4-byte segment
    //Start at byte 8 (skip the PSC Header)
    for (i=8;i<len;i=i+4) {
    	temp = inbuf[i];
    	inbuf[i] = inbuf[i + 3];
    	inbuf[i + 3] = temp;
    	temp = inbuf[i + 1];
    	inbuf[i + 1] = inbuf[i + 2];
    	inbuf[i + 2] = temp;
    }

}






void ReadADCWvfm(char *msg) {

    u32 i,j;
    u16 *msg_u16ptr;
    u32 *msg_u32ptr;
    u32 chA,chB,chC,chD;



    //write the PSC Header
     msg_u32ptr = (u32 *)msg;
     msg[0] = 'P';
     msg[1] = 'S';
     msg[2] = 0;
     msg[3] = (short int) MSGID51;
     *++msg_u32ptr = htonl(MSGID51LEN); //body length



    msg_u16ptr = (u16 *) &msg[MSGHDRLEN];
    for (i=0;i<8000;i++) {
    	for (j=0;j<8;j++) {
    		if (j<6) {
               //2 samples in a 32 bit word
    		   chA = Xil_In32(XPAR_M_AXI_BASEADDR + ADC0FIFO_DATA);
    		   chB = Xil_In32(XPAR_M_AXI_BASEADDR + ADC1FIFO_DATA);
    		   chC = Xil_In32(XPAR_M_AXI_BASEADDR + ADC2FIFO_DATA);
    		   chD = Xil_In32(XPAR_M_AXI_BASEADDR + ADC3FIFO_DATA);
    		   *msg_u16ptr++ = ((s16) ((chA & 0xFFFF0000) >> 16)) >> 2;
       		   *msg_u16ptr++ = ((s16) ((chB & 0xFFFF0000) >> 16)) >> 2;
       		   *msg_u16ptr++ = ((s16) ((chC & 0xFFFF0000) >> 16)) >> 2;
       		   *msg_u16ptr++ = ((s16) ((chD & 0xFFFF0000) >> 16)) >> 2;
    		   *msg_u16ptr++ = ((s16) (chA & 0xFFFF)) >> 2;
       		   *msg_u16ptr++ = ((s16) (chB & 0xFFFF)) >> 2;
       		   *msg_u16ptr++ = ((s16) (chC & 0xFFFF)) >> 2;
       		   *msg_u16ptr++ = ((s16) (chD & 0xFFFF)) >> 2;
    		}
    		else {//fifo has 2 blank 32 bit words
      	        Xil_In32(XPAR_M_AXI_BASEADDR + ADC0FIFO_DATA);
      	        Xil_In32(XPAR_M_AXI_BASEADDR + ADC1FIFO_DATA);
      	        Xil_In32(XPAR_M_AXI_BASEADDR + ADC2FIFO_DATA);
      	        Xil_In32(XPAR_M_AXI_BASEADDR + ADC3FIFO_DATA);

    		}

    	}
    }

    //printf("Resetting FIFO...\n");
    Xil_Out32(XPAR_M_AXI_BASEADDR + ADCFIFO_RESET, 1);
    usleep(100);
    Xil_Out32(XPAR_M_AXI_BASEADDR + ADCFIFO_RESET, 0);


}






void psc_wvfm_thread()
{

	int sockfd, newsockfd;
	int clilen;
	struct sockaddr_in serv_addr, cli_addr;
    u32 i,loopcnt=0;
    s32 n;
    u32 wdcnt;



    xil_printf("Starting PSC Waveform Server...\r\n");

	// Initialize socket structure
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

    // First call to socket() function
	if ((sockfd = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		xil_printf("PSC Waveform : Error Creating Socket\r\n");
		//vTaskDelete(NULL);
	}

    // Bind to the host address using bind()
	if (lwip_bind(sockfd, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
		xil_printf("PSC Waveform : Error Creating Socket\r\n");
		//vTaskDelete(NULL);
	}

    // Now start listening for the clients
	lwip_listen(sockfd, 0);

    xil_printf("PSC Waveform:  Server listening on port %d...\r\n",PORT);


reconnect:

	clilen = sizeof(cli_addr);

	newsockfd = lwip_accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
	if (newsockfd < 0) {
	    xil_printf("PSC Waveform: ERROR on accept\r\n");
	    //vTaskDelete(NULL);
	}
	/* If connection is established then start communicating */
	xil_printf("PSC Waveform: Connected Accepted...\r\n");
    xil_printf("PSC Waveform: Entering while loop...\r\n");


	while (1) {

		xil_printf("Wvfm: In main waveform loop...\r\n");
		loopcnt++;
		soft_trig();
		vTaskDelay(pdMS_TO_TICKS(1000));

		//read wordcnt
		wdcnt = Xil_In32(XPAR_M_AXI_BASEADDR + ADC0FIFO_WDCNT);
		//xil_printf("wdcnt = %d\r\n",wdcnt);
		if (wdcnt > 10000)  {
            xil_printf("Wvfm(%d) Sending Live Data...\r\n",loopcnt);
        	ReadADCWvfm(msgid51_buf);
        	//write out Live ADC data (msg51)
        	Host2NetworkConvWvfm(msgid51_buf,sizeof(msgid51_buf)+MSGHDRLEN);
       	    //for(i=0;i<20;i=i+4)
            //          printf("%d: %d  %d  %d  %d\n",i-8,msgid32_buf[i], msgid32_buf[i+1],
            //      		                msgid32_buf[i+2], msgid32_buf[i+3]);
        	n = write(newsockfd,msgid51_buf,MSGID51LEN+MSGHDRLEN);
        	if (n < 0) {
        		printf("PSC Waveform: ERROR writing MSG 51 - ADC Waveform   n=%d\r\n",n);
        		close(newsockfd);
        		goto reconnect;
        	}

        }
		else
			xil_printf("ADC Wfm Trigger Error...\r\n");

	}

	/* close connection */
	close(newsockfd);
	vTaskDelete(NULL);
}


