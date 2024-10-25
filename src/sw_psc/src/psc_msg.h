//The PSC interface defines many different MsgID's
//The PSC Header is always 8 bytes
#define MSGHDRLEN 8

//This message is for slow status
#define MSGID30 30
#define MSGID30LEN 1024 //748   //in bytes

//This message is for 10Hz data
#define MSGID31 31
#define MSGID31LEN 1024  //316   //in bytes

//This message is for system info
#define MSGID32 32
#define MSGID32LEN 1024  //400  //in bytes



//This message is for ADC Live waveform
//10000 pts * 4vals * 2bytes/val
#define MSGID51 51 
#define MSGID51LEN 80000   //in bytes

//This message is for TbT waveform
//1024 pts * 7vals * 4bytes/val (a,b,c,d,x,y,sum)
#define MSGID52 52 
#define MSGID52LEN 230000  //in bytes

//This message is for ADC DMA
//1e6 pts * 4vals * 2bytes/val
#define MSGID53 53
#define MSGID53LEN 8000000  //in bytes

//This message is for TBT DMA
//100e3 pts * 16vals * 4bytes/val
#define MSGID54 54
#define MSGID54LEN 6400000  //in bytes

//This message is for FA DMA
//20e3 pts * 10vals * 4bytes/val
#define MSGID55 55
#define MSGID55LEN 800000  //in bytes




//global buffers
extern char msgid30_buf[MSGID30LEN];
extern char msgid31_buf[MSGID31LEN];
extern char msgid32_buf[MSGID32LEN];

extern char msgid51_buf[MSGID51LEN];
extern char msgid52_buf[MSGID52LEN];
extern char msgid53_buf[MSGID53LEN];
extern char msgid54_buf[MSGID54LEN];
extern char msgid55_buf[MSGID55LEN];


typedef struct SAdataMsg {
	u32 count;        // PSC Offset 0
	u32 evr_ts_s;     // PSC Offset 4
	u32 evr_ts_ns;    // PSC Offset 8
	u32 cha_mag;      // PSC Offset 12
	u32 chb_mag;      // PSC Offset 16
	u32 chc_mag;      // PSC Offset 20
	u32 chd_mag;      // PSC Offset 24
	u32 sum;          // PSC Offset 28
	s32 xpos_nm;      // PSC Offset 32
	s32 ypos_nm;      // PSC Offset 36
} SAdataMsg;


typedef struct SysHealthMsg {
	u32   fpgaver;       // PSC Offset 0   Firmware Version
	float dfe_temp[4];   // PSC Offset 4   ADT7410 Temperature Sensors
	float afe_temp[2];   // PSC Offset 20
    float fpga_dietemp;  // PSC Offset 28
    u32   uptime;        // PSC Offset 32
	u32   rsvd;          // PSC Offset 36
    float vin_v;         // PSC Offset 40   LTC2991 monitor
	float vin_i;         // PSC Offset 44
	float v3_3_v;        // PSC Offset 48
	float v3_3_i;		 // PSC Offset 52
	float v2_5_v;		 // PSC Offset 56
	float v2_5_i;		 // PSC Offset 60
	float v1_8_v;		 // PSC Offset 64
	float v1_8_i;		 // PSC Offset 68
    float v1_2ddr_v;	 // PSC Offset 72
    float v1_2ddr_i;	 // PSC Offset 76
    float v0_85_v;		 // PSC Offset 80
    float v0_85_i;		 // PSC Offset 84
    float v2_5mgt_v;	 // PSC Offset 88
    float v2_5mgt_i;	 // PSC Offset 92
    float v1_2mgt_v;	 // PSC Offset 96
    float v1_2mgt_i;	 // PSC Offset 100
    float v0_9mgt_v;	 // PSC Offset 104
    float v0_9mgt_i;     // PSC Offset 108
    float reg_temp[3];   // PSC Offset 112
    u32  rsvd2[4];       // PSC Offset 124
    float sfp_temp[6];   // PSC Offset 140
    float sfp_vcc[6];    // PSC Offset 164
    float sfp_txbias[6]; // PSC Offset 188
    float sfp_txpwr[6];  // PSC Offset 212
    float sfp_rxpwr[6];  // PSC Offset 236
    u32 rsvd3[5];        // PSC Offset 260
    float pwrmgmt_temp;  // PSC Offset 280  LTC2977 Temperature
    u32   rsvd4[10];
} SysHealthMsg;


typedef struct StatusMsg {
	u32 pll_locked;         //PSC Offset 0
	u32 cha_gain;           //PSC Offset 4
    u32 chb_gain;  		    //PSC Offset 8
    u32 chc_gain;           //PSC Offset 12
    u32 chd_gain;           //PSC Offset 16
    u32 rf_atten;           //PSC Offset 20
    u32 kx;				    //PSC Offset 24
    u32 ky; 			    //PSC Offset 28
    u32 bba_xoff;           //PSC Offset 32
    u32 bba_yoff;           //PSC Offset 36
    u32 evr_ts_s_triglat;   //PSC Offset 40
	u32 evr_ts_ns_triglat;  //PSC Offset 44
    u32 trig_eventno;       //PSC Offset 48
    u32 trig_dmacnt;        //PSC Offset 52
    u32 fine_trig_dly;      //PSC Offset 56
    u32 coarse_trig_dly;    //PSC Offset 60
    u32 trigtobeam_thresh;  //PSC Offset 64
	u32 trigtobeam_dly;     //PSC Offset 68
} StatusMsg;



// Control Message Offsets
#define SOFT_TRIG_MSG1 0
#define FP_LED_MSG1 4
#define PILOT_TONE_ENB_MSG1 8
#define DMA_TRIG_SRC_MSG1 52
#define MACHINE_SEL_MSG1 76
#define PILOT_TONE_SPI_MSG1 104
#define RF_ATTEN_MSG1 132
#define PT_ATTEN_MSG1 136
#define KX_MSG1 144
#define KY_MSG1 148
#define BBA_XOFF_MSG1 152
#define BBA_YOFF_MSG1 156
#define CHA_GAIN_MSG1 160
#define CHB_GAIN_MSG1 164
#define CHC_GAIN_MSG1 168
#define CHD_GAIN_MSG1 172
#define FINE_TRIG_DLY_MSG1 192  //Geo Delay
#define TBT_GATE_WIDTH_MSG1 196
#define COARSE_TRIG_DLY_MSG1 272
#define TRIGTOBEAM_THRESH_MSG1 276
#define EVENT_NO_MSG1 320





/*
//Position Data Readback
//Inj - On Trigger
//Ring - 10Hz
#define AMPL_ASA_MSG31 20
#define AMPL_BSA_MSG31 24
#define AMPL_CSA_MSG31 28
#define AMPL_DSA_MSG31 32
#define POS_X_MSG31 36
#define POS_Y_MSG31 40
#define AMPL_SUM_MSG31 44
#define TRIG_EVENTNO_MSG31 84



// System Health Status Readback offsets
// 1Hz 
#define FPGA_VER_MSG32 0 
#define TEMP_DFESENSE0_MSG32 4 
#define TEMP_DFESENSE1_MSG32 8 
#define TEMP_DFESENSE2_MSG32 12 
#define TEMP_DFESENSE3_MSG32 16 
#define TEMP_AFESENSE0_MSG32 20 
#define TEMP_AFESENSE1_MSG32 24
#define FPGA_DIETEMP_MSG32 28
#define SYS_UPTIME_MSG32 32
#define LTC2991_VCC_VIN_MSG32 40
#define SFP_TEMP_MSG32 100  // to 216
#define PWR_MON_MSG32 220
*/
