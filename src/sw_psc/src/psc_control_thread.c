

#include <stdio.h>
#include <string.h>

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


#define PORT  7



void set_fpleds(volatile unsigned int *fpgabase, int msgVal)  {
	fpgabase[FP_LEDS_REG] = msgVal;
}


//void soft_trig(volatile unsigned int *fpgabase, int msgVal) {
//	Xil_Out32(XPAR_M_AXI_BASEADDR + ADCFIFO_TRIG, msgVal);
//}

void set_atten(volatile unsigned int *fpgabase, int msgVal) {
    xil_printf("Setting RF attenuator to %d dB\r\n",msgVal);
    fpgabase[RF_DSA_REG] = msgVal*4;
}

void set_geo_dly(volatile unsigned int *fpgabase, int msgVal) {
    // the Geo delay is the same as tbt_gate delay, set them both for now
    fpgabase[FINE_TRIG_DLY_REG] = msgVal;
    fpgabase[TBT_GATEDLY_REG] = msgVal;
}


void set_coarse_dly(volatile unsigned int *fpgabase, int msgVal) {
    fpgabase[COARSE_TRIG_DLY_REG] = msgVal;
}



void set_trigtobeam_thresh(volatile unsigned int *fpgabase, int msgVal) {
    fpgabase[TRIGTOBEAM_THRESH_REG] = msgVal;
}


void set_eventno(volatile unsigned int *fpgabase, int msgVal) {
    fpgabase[EVR_DMA_TRIGNUM_REG] = msgVal;
}

void set_trigsrc(volatile unsigned int *fpgabase, int msgVal) {
    if (msgVal == 0) {
        xil_printf("Setting Trigger Source to EVR\r\n");
        fpgabase[TRIG_EVRINT_SEL_REG] = msgVal;
    }
    else if (msgVal == 1) {
	    xil_printf("Setting Trigger Source to INT (soft)\r\n");
        fpgabase[TRIG_EVRINT_SEL_REG] = msgVal;
    }
    else
        xil_printf("Invalid Trigger Source\r\n");

}



void set_kxky(volatile unsigned int *fpgabase, int axis, int msgVal) {

    if (axis == HOR)	{
       xil_printf("Setting Kx to %d nm\r\n",msgVal);
       fpgabase[KX_REG] = msgVal;
    }
    else {
       xil_printf("Setting Ky to %d nm\r\n",msgVal);
       fpgabase[KY_REG] = msgVal;
    }
}


void set_bbaoffset(volatile unsigned int *fpgabase, int axis, int msgVal) {

    if (axis == HOR)	{
       xil_printf("Setting BBA X to %d nm\r\n",msgVal);
       fpgabase[BBA_XOFF_REG] = msgVal;
    }
    else {
       xil_printf("Setting BBA Y to %d nm\r\n",msgVal);
       fpgabase[BBA_YOFF_REG] = msgVal;
    }
}




void set_gain(volatile unsigned int *fpgabase, int channel, int msgVal) {


switch(channel) {
    case CHA:
	   fpgabase[CHA_GAIN_REG] = msgVal;
       xil_printf("Setting ChA gain to %d \r\n",msgVal);
	   break;
	case CHB:
	   fpgabase[CHB_GAIN_REG] = msgVal;
       xil_printf("Setting ChB gain to %d\r\n",msgVal);
	   break;
    case CHC:
	   fpgabase[CHC_GAIN_REG] = msgVal;
       xil_printf("Setting ChC gain to %d\r\n",msgVal);
	   break;
	case CHD:
	   fpgabase[CHD_GAIN_REG] = msgVal;
       xil_printf("Setting ChD gain to %d\r\n",msgVal);
	   break;
    default:
       xil_printf("Invalid gain channel number\r\n");
	   break;
    }
}





void psc_control_thread()
{
	int sockfd, newsockfd;
	int clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int RECV_BUF_SIZE = 1024;
	char buffer[RECV_BUF_SIZE];
	int n, *bufptr, numpackets=0;
    int MsgAddr, MsgData;

	unsigned int *fpgabase;
	fpgabase = (unsigned int *) IOBUS_BASEADDR;


    xil_printf("Starting PSC Control Server...\r\n");

	// Initialize socket structure
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

    // First call to socket() function
	if ((sockfd = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		xil_printf("PSC Control : Error Creating Socket\r\n");

    // Bind to the host address using bind()
	if (lwip_bind(sockfd, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0)
		xil_printf("PSC Control : Error Creating Socket\r\n");

    // Now start listening for the clients
	lwip_listen(sockfd, 0);


    xil_printf("PSC Control: Server listening on port %d...\r\n",PORT);


reconnect:

	clilen = sizeof(cli_addr);

	newsockfd = lwip_accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
	if (newsockfd < 0) {
	    xil_printf("PSC Control: ERROR on accept\r\n");

	}
	/* If connection is established then start communicating */
	xil_printf("PSC Control: Connected Accepted...\r\n");

	while (1) {
		/* read a max of RECV_BUF_SIZE bytes from socket */
		n = read(newsockfd, buffer, RECV_BUF_SIZE);
        if (n <= 0) {
            xil_printf("PSC Control: ERROR reading from socket..  Reconnecting...\r\n");
            close(newsockfd);
	        goto reconnect;
        }

        bufptr = (int *) buffer;
        xil_printf("\nPacket %d Received : NumBytes = %d\r\n",++numpackets,n);
        xil_printf("Header: %c%c \t",buffer[0],buffer[1]);
        xil_printf("Message ID : %d\t",(ntohl(*bufptr++)&0xFFFF));
        xil_printf("Body Length : %d\t",ntohl(*bufptr++));
        MsgAddr = ntohl(*bufptr++);
        xil_printf("Msg Addr : %d\t",MsgAddr);
	    MsgData = ntohl(*bufptr);
        xil_printf("Data : %d\r\n",MsgData);


        switch(MsgAddr) {
			case SOFT_TRIG_MSG1:
				xil_printf("Soft Trigger Message:   Value=%d\r\n",MsgData);
                soft_trig(fpgabase,MsgData);
                break;

			case DMA_TRIG_SRC_MSG1:
				xil_printf("Set Trigger Source Message:   Value=%d\r\n",MsgData);
                set_trigsrc(fpgabase,MsgData);
                break;

			case RF_ATTEN_MSG1:
				xil_printf("RF Attenuator Message:   Value=%d\r\n",MsgData);
                set_atten(fpgabase,MsgData);
                break;

			case KX_MSG1:
				xil_printf("Kx Message:   Value=%d\r\n",MsgData);
                set_kxky(fpgabase,HOR,MsgData);
                break;

			case KY_MSG1:
				xil_printf("Ky Message:   Value=%d\r\n",MsgData);
                set_kxky(fpgabase,VERT,MsgData);
                break;

            case BBA_XOFF_MSG1:
            	xil_printf("BBA X Offset:   Value=%d\r\n",MsgData);
            	set_bbaoffset(fpgabase,HOR,MsgData);
                break;

            case BBA_YOFF_MSG1:
            	xil_printf("BBA Y Offset:   Value=%d\r\n",MsgData);
            	set_bbaoffset(fpgabase,VERT,MsgData);
                break;

            case CHA_GAIN_MSG1:
            	xil_printf("ChA Gain Message:   Value=%d\r\n",MsgData);
                set_gain(fpgabase,CHA,MsgData);
                break;

            case CHB_GAIN_MSG1:
            	xil_printf("ChB Gain Message:   Value=%d\r\n",MsgData);
                set_gain(fpgabase,CHB,MsgData);
                break;

            case CHC_GAIN_MSG1:
            	xil_printf("ChC Gain Message:   Value=%d\r\n",MsgData);
                set_gain(fpgabase,CHC,MsgData);
                break;

            case CHD_GAIN_MSG1:
            	xil_printf("ChD Gain Message:   Value=%d\r\n",MsgData);
                set_gain(fpgabase,CHD,MsgData);
                break;

            case FINE_TRIG_DLY_MSG1:
            	xil_printf("Fine Trig Delay Message:   Value=%d\r\n",MsgData);
                set_geo_dly(fpgabase,MsgData);
                break;

            case COARSE_TRIG_DLY_MSG1:
            	xil_printf("Coarse Trig Delay Message:   Value=%d\r\n",MsgData);
            	set_coarse_dly(fpgabase,MsgData);
            	break;

            case TRIGTOBEAM_THRESH_MSG1:
            	xil_printf("Trigger to Beam Threshold Message:   Value=%d\r\n",MsgData);
            	set_trigtobeam_thresh(fpgabase,MsgData);
            	break;


            case EVENT_NO_MSG1:
            	xil_printf("TbT Gate Width Message:   Value=%d\r\n",MsgData);
                set_eventno(fpgabase,MsgData);
                break;


            case FP_LED_MSG1:
            	xil_printf("Setting FP LED:   Value=%d\r\n",MsgData);
            	set_fpleds(fpgabase,MsgData);
            	break;

            default:
            	xil_printf("Msg not supported yet...\r\n");
            	break;
        }

	}

	/* close connection */
	close(newsockfd);
	vTaskDelete(NULL);
}


