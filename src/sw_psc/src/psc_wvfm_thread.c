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


void dma_arm(u32 *fpgabase, u32 *dmaadcbase, u32 *dmatbtbase, u32 *dmafabase, u32 dma_adclen, u32 dma_tbtlen, u32 dma_falen) {

	u32 i;
	u32 *adc_ptr, *tbt_ptr, *fa_ptr;

	xil_printf("Arming DMA...\r\n");

	//clear the DMA memory
	adc_ptr = (u32 *) ADC_DMA_DATA;
	tbt_ptr = (u32 *) TBT_DMA_DATA;
	fa_ptr  = (u32 *) FA_DMA_DATA;
	for (i=0;i<10000000;i++)  adc_ptr[i] = 0;
	for (i=0;i<10000000;i++)  tbt_ptr[i] = 0;
	for (i=0;i<10000000;i++)  fa_ptr[i]  = 0;


	//reset the PL DMA FIFO
	fpgabase[DMA_FIFORST_REG] = 1;
	fpgabase[DMA_FIFORST_REG] = 0;

	//reset the AXI DMA Core
	dmaadcbase[S2MM_DMACR] = 4;
	dmatbtbase[S2MM_DMACR] = 4;
	dmafabase[S2MM_DMACR]  = 4;

	//write the number of ADC samples to DMA
	fpgabase[DMA_ADCBURSTLEN_REG] = dma_adclen+16;
	fpgabase[DMA_TBTBURSTLEN_REG] = dma_tbtlen;
	fpgabase[DMA_FABURSTLEN_REG]  = dma_falen;

	//Start the S2MM channel with all interrupts masked
	dmaadcbase[S2MM_DMACR] = 0xF001;
	dmatbtbase[S2MM_DMACR] = 0xF001;
	dmafabase[S2MM_DMACR]  = 0xF001;

	//Write the Destination Address for the ADC data
	dmaadcbase[S2MM_DA] = ADC_DMA_DATA;
    dmatbtbase[S2MM_DA] = TBT_DMA_DATA;
    dmafabase[S2MM_DA]  = FA_DMA_DATA;

	//Write the S2MM transfer length (must be written last (PG021 p72)
    //length is in bytes, for adc: 4 adc channels * 2bytes/sample
	dmaadcbase[S2MM_LEN] = (dma_adclen+16) * 4 * 2;
	//length is in bytes, for TbT: 16 - 4 byte values
    dmatbtbase[S2MM_LEN] = (dma_tbtlen) * 16 * 4;
	//length is in bytes, for FA: 10 - 4 byte values
    dmafabase[S2MM_LEN] = (dma_tbtlen) * 10 * 4;

	//Enable the ADC and TbT DMA
	xil_printf("Enabling the DMA\r\n");
	//bit0=ADC enable,  bit1=TbT enable, bit2=FA enable
	fpgabase[DMA_ENABLE_REG] = 7;

}



void ReadDMAADCWvfm(char *msg) {

    u32 *adc_data;
    u32 *msg_u32ptr;
    u32 i;
	s16 adc_cha, adc_chb, adc_chc, adc_chd;

	xil_printf("\r\nADC Data\r\n");
	//print some debug information
    adc_data = (u32 *) ADC_DMA_DATA;
	for (i=0;i<20;i=i+2) {
	   adc_chb = (short int) ((adc_data[i] & 0xFFFF0000) >> 16);
	   adc_cha = (short int) (adc_data[i] & 0xFFFF);
	   adc_chd = (short int) ((adc_data[i+1] & 0xFFFF0000) >> 16);
	   adc_chc = (short int) (adc_data[i+1] & 0xFFFF);
	   xil_printf("%d: %d\t%d\t%d\t%d\r\n", i,adc_cha, adc_chb, adc_chc, adc_chd);
	}

    //write the PSC Header
    msg_u32ptr = (u32 *)msg;
    msg[0] = 'P';
    msg[1] = 'S';
    msg[2] = 0;
    msg[3] = (short int) MSGID53;
    *++msg_u32ptr = htonl(MSGID53LEN); //body length
	msg_u32ptr++;

    //copy the DMA'd ADC data into the msgid53 buffer
    adc_data = (u32 *) ADC_DMA_DATA;
	for (i=0;i<MSGID53LEN/4;i++) {
	    *msg_u32ptr++ = *adc_data++;
	     }
}




void ReadDMATBTWvfm(char *msg) {

    u32 *tbt_data;
    u32 *msg_u32ptr;
    u32 i, hdr, cnt;
	s32 cha_mag, chb_mag, chc_mag, chd_mag, sum, xpos_nm, ypos_nm;
	//s32 rsvd, cha_phs, chb_phs, chc_phs, chd_phs, xpos, ypos;

	xil_printf("\r\nTbT Data\r\n");
    tbt_data = (u32 *) TBT_DMA_DATA;
	for (i=0;i<16*10;i=i+16) {
	   hdr     = (u32) tbt_data[i];
	   cnt     = (u32) tbt_data[i+1];
	   cha_mag = (s32) tbt_data[i+2];
	   //cha_phs = (s32) tbt_data[i+3];
	   chb_mag = (s32) tbt_data[i+4];
	   //chb_phs = (s32) tbt_data[i+5];
	   chc_mag = (s32) tbt_data[i+6];
	   //chc_phs = (s32) tbt_data[i+7];
	   chd_mag = (s32) tbt_data[i+8];
	   //chd_phs = (s32) tbt_data[i+9];
	   //xpos    = (s32) tbt_data[i+10];
	   //ypos    = (s32) tbt_data[i+11];
	   //rsvd    = (s32) tbt_data[i+12];
	   sum     = (s32) tbt_data[i+13];
	   xpos_nm = (s32) tbt_data[i+14];
	   ypos_nm = (s32) tbt_data[i+15];
	   xil_printf("%8x\t%8d \t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", hdr, cnt,
			   	   	   	  cha_mag, chb_mag, chc_mag, chd_mag, sum, xpos_nm, ypos_nm);
	   }

    //write the PSC Header
    msg_u32ptr = (u32 *)msg;
    msg[0] = 'P';
    msg[1] = 'S';
    msg[2] = 0;
    msg[3] = (short int) MSGID54;
    *++msg_u32ptr = htonl(MSGID54LEN); //body length
	msg_u32ptr++;

    //copy the DMA'd ADC data into the msgid54 buffer
    tbt_data = (u32 *) TBT_DMA_DATA;
	for (i=0;i<MSGID54LEN/4;i++) {
	    *msg_u32ptr++ = *tbt_data++;
	     }


}



void ReadDMAFAWvfm(char *msg) {

    u32 *fa_data;
    u32 *msg_u32ptr;
    u32 i, hdr, cnt;
	s32 cha_mag, chb_mag, chc_mag, chd_mag, sum, xpos_nm, ypos_nm;
	//s32 rsvd, cha_phs, chb_phs, chc_phs, chd_phs, xpos, ypos;

	xil_printf("\r\nFA Data\r\n");
    fa_data = (u32 *) FA_DMA_DATA;
	for (i=0;i<10*10;i=i+10) {
	   hdr     = (u32) fa_data[i];
	   cnt     = (u32) fa_data[i+1];
	   cha_mag = (s32) fa_data[i+2];
	   chb_mag = (s32) fa_data[i+3];
	   chc_mag = (s32) fa_data[i+4];
	   chd_mag = (s32) fa_data[i+5];
	   sum     = (s32) fa_data[i+6];
	   xpos_nm = (s32) fa_data[i+8];
	   ypos_nm = (s32) fa_data[i+9];
	   xil_printf("%8x\t%8d \t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", hdr, cnt,
			   	   	   	  cha_mag, chb_mag, chc_mag, chd_mag, sum, xpos_nm, ypos_nm);
	   }

    //write the PSC Header
    msg_u32ptr = (u32 *)msg;
    msg[0] = 'P';
    msg[1] = 'S';
    msg[2] = 0;
    msg[3] = (short int) MSGID55;
    *++msg_u32ptr = htonl(MSGID55LEN); //body length
	msg_u32ptr++;

    //copy the DMA'd FA data into the msgid55 buffer
    fa_data = (u32 *) FA_DMA_DATA;
	for (i=0;i<MSGID55LEN/4;i++) {
	    *msg_u32ptr++ = *fa_data++;
	     }


}





void ReadLiveTbTWvfm(volatile unsigned int *fpgabase, char *msg) {

    int i;
    u32 *msg_u32ptr;


    fpgabase[TBTFIFO_STREAMENB_REG] = 1;
    fpgabase[TBTFIFO_STREAMENB_REG] = 0;
    usleep(30000);

    //printf("Reading TbT FIFO...\n");
    //printf("\tWords in TbT FIFO = %d\n",fpgabase[TBTFIFO_CNT_REG]);

    //write the PSC Header
     msg_u32ptr = (u32 *)msg;
     msg[0] = 'P';
     msg[1] = 'S';
     msg[2] = 0;
     msg[3] = (short int) MSGID52;
     *++msg_u32ptr = htonl(MSGID52LEN); //body length
 	 msg_u32ptr++;


    // Get TbT Waveform
    for (i=0;i<8000*2;i++)
	   *msg_u32ptr++ = fpgabase[TBTFIFO_DATA_REG];

    //printf("TbT FIFO Read Complete...\n");
    //printf("Resetting FIFO...\n");
    fpgabase[TBTFIFO_RST_REG] = 0x1;
    usleep(1);
    fpgabase[TBTFIFO_RST_REG] = 0x0;
    usleep(10);

}



void ReadLiveADCWvfm(volatile unsigned int *fpgabase, char *msg) {

    int i;
    u16 *msg_u16ptr;
    u32 *msg_u32ptr;
    //int hdr, ts_s, ts_ns;
    int regVal;


    fpgabase[ADCFIFO_STREAMENB_REG] = 1;
    fpgabase[ADCFIFO_STREAMENB_REG] = 0;
    usleep(10000);
    //xil_printf("Reading ADC FIFO...\r\n");
    //xil_printf("\tWords in ADC FIFO = %d\r\n",fpgabase[ADCFIFO_CNT_REG]);
    // first 2 words is 0x80000000 and 0x00000000
    //xil_printf("\tADC header: %x\r\n",fpgabase[ADCFIFO_DATA_REG]);
    //hdr = fpgabase[ADCFIFO_DATA_REG];
    // next 2 words are the timestamp trigger
    //ts_s = fpgabase[ADCFIFO_DATA_REG];
    //ts_ns = fpgabase[ADCFIFO_DATA_REG];
    //xil_printf("\tTrigger Timestamp: %d   %d\r\n",ts_s,ts_ns);


    //write the PSC Header
     msg_u32ptr = (u32 *)msg;
     msg[0] = 'P';
     msg[1] = 'S';
     msg[2] = 0;
     msg[3] = (short int) MSGID51;
     *++msg_u32ptr = htonl(MSGID51LEN); //body length



    msg_u16ptr = (u16 *) &msg[MSGHDRLEN];
    for (i=0;i<8000;i++) {
        //chA and chB are in a single 32 bit word
    	regVal = fpgabase[ADCFIFO_DATA_REG];
        *msg_u16ptr++ = (short int) ((regVal & 0xFFFF0000) >> 16);
        *msg_u16ptr++ = (short int) (regVal & 0xFFFF);
        //chC and chD are in a single 32 bit word
        regVal = fpgabase[ADCFIFO_DATA_REG];
        *msg_u16ptr++ = (short int) ((regVal & 0xFFFF0000) >> 16);
        *msg_u16ptr++ = (short int) (regVal & 0xFFFF);
    }

    //printf("Resetting FIFO...\n");
    fpgabase[ADCFIFO_RST_REG] = 0x1;
    usleep(1);
    fpgabase[ADCFIFO_RST_REG] = 0x0;
    usleep(10);


}






void psc_wvfm_thread()
{

	int sockfd, newsockfd;
	int clilen;
	struct sockaddr_in serv_addr, cli_addr;
    u32 loopcnt=0;
    s32 n;
    u32 dma_adclen, dma_tbtlen, dma_falen;
	u32 *fpgaiobase, *fpgalivebase, *dmaadcbase, *dmatbtbase, *dmafabase;
    u32 trignum=0, prevtrignum=0;

    // Initialize pointers to various memory locations in the PL fabric
	fpgaiobase   = (u32 *) IOBUS_BASEADDR;      //PL Memory for General Registers
	fpgalivebase = (u32 *) LIVEBUS_BASEADDR;    //PL Memory for Live Data
	dmaadcbase   = (u32 *) AXIDMA_ADCBASEADDR;  //AXI DMA Core for ADC Data
	dmatbtbase   = (u32 *) AXIDMA_TBTBASEADDR;  //AXI DMA Core for TbT Data
    dmafabase    = (u32 *) AXIDMA_FABASEADDR;   //AXI DMA Core for FA Data

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

    //Set DMA ADC data to test pattern (counter)
    //fpgaiobase[DMA_TESTDATAENB_REG] = 1;

    dma_adclen = 1e6;
    dma_tbtlen = 100e3;
    dma_falen  = 20e3;  // 2 seconds
    dma_arm(fpgaiobase, dmaadcbase, dmatbtbase, dmafabase, dma_adclen, dma_tbtlen, dma_falen);


	while (1) {

		//xil_printf("Wvfm: In main waveform loop...\r\n");
		loopcnt++;
		vTaskDelay(pdMS_TO_TICKS(100));

		trignum = fpgaiobase[DMA_TRIGCNT_REG];
		if (trignum != prevtrignum)  {
			//received a DMA trigger
            printf("\nTrig Num: %d  \n",trignum);
            prevtrignum = trignum;
 	        Xil_DCacheInvalidateRange(ADC_DMA_DATA,dma_adclen*8);
 	        Xil_DCacheInvalidateRange(TBT_DMA_DATA,dma_tbtlen*16*4);


            //Read and send the TbT DMA data
 	        ReadDMATBTWvfm(msgid54_buf);
            //write the DMA data
            Host2NetworkConvWvfm(msgid54_buf,sizeof(msgid54_buf)+MSGHDRLEN);
            n = write(newsockfd,msgid54_buf,MSGID54LEN+MSGHDRLEN);
            xil_printf("TbTWvfm bytes Written: %d\r\n",n);
            if (n < 0) {
            	xil_printf("PSC Waveform: ERROR writing MSG 54 - TBT DMA Waveform\r\n");
            	close(newsockfd);
            	goto reconnect;
            }

 	        //Read and send the ADC DMA data
 	        ReadDMAADCWvfm(msgid53_buf);
            Host2NetworkConvWvfm(msgid53_buf,sizeof(msgid53_buf)+MSGHDRLEN);
            n = write(newsockfd,msgid53_buf,MSGID53LEN+MSGHDRLEN);
            xil_printf("ADCWvfm bytes Written: %d\r\n",n);
            if (n < 0) {
            	xil_printf("PSC Waveform: ERROR writing MSG 53 - ADC DMA Waveform\r\n");
            	close(newsockfd);
            	goto reconnect;
            }

	        //Read and send the FA DMA data
 	        ReadDMAFAWvfm(msgid55_buf);
            Host2NetworkConvWvfm(msgid55_buf,sizeof(msgid55_buf)+MSGHDRLEN);
            n = write(newsockfd,msgid55_buf,MSGID55LEN+MSGHDRLEN);
            xil_printf("FAWvfm bytes Written: %d\r\n",n);
            if (n < 0) {
            	xil_printf("PSC Waveform: ERROR writing MSG 55 - FA DMA Waveform\r\n");
            	close(newsockfd);
            	goto reconnect;
            }




            //re-arm AXI DMA for next trigger
            dma_arm(fpgaiobase, dmaadcbase, dmatbtbase, dmafabase, dma_adclen, dma_tbtlen, dma_falen);

		}


        if (loopcnt % 10 == 0) {
            //xil_printf("Wvfm(%d) Sending Live Data...\r\n",loopcnt);
        	ReadLiveADCWvfm(fpgalivebase,msgid51_buf);
        	//write out Live ADC data (msg51)
        	Host2NetworkConvWvfm(msgid51_buf,sizeof(msgid51_buf)+MSGHDRLEN);
        	n = write(newsockfd,msgid51_buf,MSGID51LEN+MSGHDRLEN);
        	if (n < 0) {
        		printf("PSC Waveform: ERROR writing MSG 51 - ADC Waveform\n");
        		close(newsockfd);
        		goto reconnect;
        	}


        	//xil_printf("%8d:  Reading TbT Waveform...\r\n",loopcnt);
        	ReadLiveTbTWvfm(fpgalivebase,msgid52_buf);
        	//write out Live TbT data (msg52)
        	Host2NetworkConvWvfm(msgid52_buf,sizeof(msgid52_buf)+MSGHDRLEN);
        	n = write(newsockfd,msgid52_buf,MSGID52LEN+MSGHDRLEN);
        	if (n < 0) {
        		printf("PSC Waveform: ERROR writing MSG 52 - TbT Waveform\n");
        		close(newsockfd);
        		goto reconnect;
        	}

        }



	}

	/* close connection */
	close(newsockfd);
	vTaskDelete(NULL);
}


