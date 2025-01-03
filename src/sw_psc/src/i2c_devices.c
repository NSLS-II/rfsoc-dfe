#include "xparameters.h"
#include "xiicps.h"
#include "rfsoc-dfe_defs.h"
#include <sleep.h>
#include "xil_printf.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"


extern XIicPs IicPsInstance;			/* Instance of the IIC Device */


static const u32 lmk61e2_values [] = {
		0x0010, 0x010B, 0x0233, 0x08B0, 0x0901, 0x1000, 0x1180, 0x1502,
		0x1600, 0x170F, 0x1900, 0x1A2E, 0x1B00, 0x1C00, 0x1DA9, 0x1E00,
		0x1F00, 0x20C8, 0x2103, 0x2224, 0x2327, 0x2422, 0x2502, 0x2600,
		0x2707, 0x2F00, 0x3000, 0x3110, 0x3200, 0x3300, 0x3400, 0x3500,
		0x3800, 0x4802,
};



// clkin1 (100MHz) dualloop clkout11 = 100MHz, clkout12 = 240MHz
static const u32 lmk_values[] = {
		0x000090, 0x000000, 0x000200, 0x000306, 0x0004D0, 0x00055B, 0x000600, 0x000C51,
		0x000D04, 0x010006, 0x010155, 0x010255, 0x010300, 0x010402, 0x010500, 0x010678,
		0x010755, 0x010806, 0x010955, 0x010A55, 0x010B00, 0x010C02, 0x010D00, 0x010E78,
		0x010F55, 0x011006, 0x011155, 0x011255, 0x011300, 0x011402, 0x011500, 0x0116F9,
		0x011700, 0x011806, 0x011955, 0x011A55, 0x011B00, 0x011C02, 0x011D00, 0x011E79,
		0x011F03, 0x012006, 0x012155, 0x012255, 0x012300, 0x012402, 0x012500, 0x012670,
		0x012700, 0x012818, 0x012955, 0x012A55, 0x012B00, 0x012C02, 0x012D00, 0x012E70,
		0x012F60, 0x01300A, 0x013155, 0x013255, 0x013300, 0x013402, 0x013500, 0x013670,
		0x013701, 0x013800, 0x013903, 0x013A04, 0x013B00, 0x013C00, 0x013D08, 0x013E03,
		0x013F00, 0x014009, 0x014100, 0x014200, 0x014301, 0x0144FF, 0x01457F, 0x014612,
		0x014703, 0x014802, 0x014902, 0x014A02, 0x014B16, 0x014C00, 0x014D00, 0x014EC0,
		0x014F7F, 0x015003, 0x015102, 0x015200, 0x015300, 0x015464, 0x015500, 0x0156C8,
		0x015700, 0x015854, 0x015900, 0x015AA0, 0x015BD4, 0x015C20, 0x015D00, 0x015E00,
		0x015F0B, 0x016000, 0x016102, 0x016224, 0x016300, 0x016400, 0x016501, 0x0171AA,
		0x017202, 0x017C15, 0x017D33, 0x016600, 0x016700, 0x01680F, 0x016959, 0x016A20,
		0x016B00, 0x016C00, 0x016D00, 0x016E1B, 0x017300, 0x1FFD00, 0x1FFE00, 0x1FFF53,
		0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
};

void init_i2c() {
    //s32 Status;
    XIicPs_Config *ConfigPtr;


    // Look up the configuration in the config table
    ConfigPtr = XIicPs_LookupConfig(0);
    //if(ConfigPtr == NULL) return XST_FAILURE;

    // Initialize the II2 driver configuration
    XIicPs_CfgInitialize(&IicPsInstance, ConfigPtr, ConfigPtr->BaseAddress);
    //if(Status != XST_SUCCESS) return XST_FAILURE;

    //set i2c clock rate to 100KHz
    XIicPs_SetSClk(&IicPsInstance, 100000);
}






s32 i2c_write(u8 *buf, u8 len, u8 addr) {

	s32 status;

	while (XIicPs_BusIsBusy(&IicPsInstance));
	status = XIicPs_MasterSendPolled(&IicPsInstance, buf, len, addr);
	return status;
}

s32 i2c_read(u8 *buf, u8 len, u8 addr) {

	s32 status;

	while (XIicPs_BusIsBusy(&IicPsInstance));
    status = XIicPs_MasterRecvPolled(&IicPsInstance, buf, len, addr);
    return status;

}








void i2c_set_port_expander(u32 addr, u32 port)  {

    u8 buf[3];
    u32 len=1;

    buf[0] = port;
    buf[1] = 0;
    buf[2] = 0;

	while (XIicPs_BusIsBusy(&IicPsInstance));
    XIicPs_MasterSendPolled(&IicPsInstance, buf, len, addr);
}


float read_i2c_temp(u8 addr) {

    u8 buf[3];
    u32 temp;
    float tempflt;

	while (XIicPs_BusIsBusy(&IicPsInstance));
    XIicPs_MasterRecvPolled(&IicPsInstance, buf, 2, addr);
    temp = (buf[0] << 8) | (buf[1]);
    tempflt = (float)temp/128.0;
    return tempflt;

}

void write_lmk61e2()
{
   u8 buf[4] = {0};
   u32 regval, i;


   u32 num_values = sizeof(lmk61e2_values) / sizeof(lmk61e2_values[0]);  // Get the number of elements in the array

   i2c_set_port_expander(I2C_PORTEXP1_ADDR,0x20);
   for (i=0; i<num_values; i++) {
	  regval = lmk61e2_values[i];
      buf[0] = (char) ((regval & 0x00FF00) >> 8);
      buf[1] = (char) (regval & 0xFF);
      i2c_write(buf,2,0x5A);
      printf("LMK61e2 Write = 0x%x\t    B0 = %x    B1 = %x\n",regval, buf[0], buf[1]);
   }

}







void read_board_temps() {

   float temp;

   i2c_set_port_expander(I2C_PORTEXP1_ADDR,1);
   temp = read_i2c_temp(BRDTEMP0_ADDR);
   printf("Board Temp0: %5.2f\r\n", temp);
   temp = read_i2c_temp(BRDTEMP1_ADDR);
   printf("Board Temp1: %5.2f\r\n", temp);
   temp = read_i2c_temp(BRDTEMP2_ADDR);
   printf("Board Temp2: %5.2f\r\n", temp);
   temp = read_i2c_temp(BRDTEMP3_ADDR);
   printf("Board Temp3: %5.2f\r\n", temp);

}



void i2c_get_ltc2991()
{

  i2c_set_port_expander(I2C_PORTEXP1_ADDR,4);

  printf("LTC2991 Voltage/Current/Temperature Monitoring\r\n");
  printf("----\r\n");
  printf("V 12.0:        %0.2f\r\n", i2c_ltc2991_vcc_vin());
  printf("I 12.0:        %0.2f\r\n", i2c_ltc2991_vcc_vin_current());
  printf("V 3.3:         %0.2f\r\n", i2c_ltc2991_vcc_3v3());
  printf("I 3.3:         %0.2f\r\n", i2c_ltc2991_vcc_3v3_current());
  printf("V 2.5:         %0.2f\r\n", i2c_ltc2991_vcc_2v5());
  printf("I 2.5:         %0.2f\r\n", i2c_ltc2991_vcc_2v5_current());
  printf("V 1.8:         %0.2f\r\n", i2c_ltc2991_vcc_1v8());
  printf("I 1.8:         %0.2f\r\n", i2c_ltc2991_vcc_1v8_current());
  printf("V 0.85:        %0.2f\r\n", i2c_ltc2991_vcc_0v85());
  printf("I 0.85:        %0.2f\r\n", i2c_ltc2991_vcc_0v85_current());
  printf("V 2.5 MGT:     %0.2f\r\n", i2c_ltc2991_vcc_mgt_2v5());
  printf("I 2.5 MGT:     %0.2f\r\n", i2c_ltc2991_vcc_mgt_2v5_current());
  printf("V 1.2 MGT:     %0.2f\r\n", i2c_ltc2991_vcc_mgt_1v2());
  printf("I 1.2 MGT:     %0.2f\r\n", i2c_ltc2991_vcc_mgt_1v2_current());
  printf("V 0.9 MGT:     %0.2f\r\n", i2c_ltc2991_vcc_mgt_0v9());
  printf("I 0.9 MGT:     %0.2f\r\n", i2c_ltc2991_vcc_mgt_0v9_current());
  printf("V 1.2 DDR:     %0.2f\r\n", i2c_ltc2991_vcc_1v2_ddr());
  printf("I 1.2 DDR:     %0.2f\r\n", i2c_ltc2991_vcc_1v2_ddr_current());
  printf("V ADC AVCC:    %0.2f\r\n", i2c_ltc2991_vcc_adc_avcc());
  printf("I ADC AVCC:    %0.2f\r\n", i2c_ltc2991_vcc_adc_avcc_current());
  printf("V ADC AVCCAUX: %0.2f\r\n", i2c_ltc2991_vcc_adc_avccaux());
  printf("I ADC AVCCAUX: %0.2f\r\n", i2c_ltc2991_vcc_adc_avccaux_current());
  printf("V DAC AVCC:    %0.2f\r\n", i2c_ltc2991_vcc_dac_avcc());
  printf("I DAC AVCC:    %0.2f\r\n", i2c_ltc2991_vcc_dac_avcc_current());
  printf("V DAC AVCCAUX: %0.2f\r\n", i2c_ltc2991_vcc_dac_avccaux());
  printf("I DAC AVCCAUX: %0.2f\r\n", i2c_ltc2991_vcc_dac_avccaux_current());
  printf("V DAC AVTT:    %0.2f\r\n", i2c_ltc2991_vcc_dac_avtt());
  printf("I DAC AVTT:    %0.2f\r\n", i2c_ltc2991_vcc_dac_avtt_current());


  printf("reg1_temp:  %0.2f\r\n", i2c_ltc2991_reg1_temp());
  printf("reg2_temp:  %0.2f\r\n", i2c_ltc2991_reg2_temp());
  printf("reg3_temp:  %0.2f\r\n", i2c_ltc2991_reg3_temp());
  printf("reg4_temp:  %0.2f\r\n", i2c_ltc2991_reg4_temp());
  printf("----\r\n");
}


void WriteLMK04828()
{
   u8 buf[4] = {0};
   u32 i;

   u32 regval;
   u32 num_values = sizeof(lmk_values) / sizeof(lmk_values[0]);  // Get the number of elements in the array


   i2c_set_port_expander(I2C_PORTEXP2_ADDR,8);
   for (i=0; i<num_values; i++) {
       regval =  lmk_values[i];
       buf[0] = 2;  //function ID, selects SS1 which is the LMK part
       buf[1] = (char) ((regval & 0xFF0000) >> 16);
       buf[2] = (char) ((regval & 0x00FF00) >> 8);
       buf[3] = (char) (regval & 0xFF);
       //printf("Regval = 0x%x\t    B0 = %x    B1 = %x   B2 = %x\n",regval, buf[1], buf[2], buf[3]);
       i2c_write(buf, 4, 0x2F);  //clk104 i2c address (SC181S602B)
   
   }
   //bytesWritten = write(dev,buf,4);
   //usleep(10000);
   //bytesRead = read(dev,buf,4);
   //printf("DAC Read : BytesRead:%d     B1: %x   B2: %x  B3: %x   B4: %x\n",bytesRead,buf[0],buf[1],buf[2],buf[3]);

}







// LTC2991  ------------------------------------------------
//static const u8 iic_ltc2991_addrs[] = {0x90>>1, 0x94>>1, 0x92>>1, 0x96>>1}; //Sorted by schematic order
/**
 * LTC2991[0] U42
 *   V1 - reg1 temp (T[deg C] = (650mV - V) / 2 mV/K)
 *   V2 - reg2 temp
 *   V3 - reg3 temp
 *   V4 - NC
 *   V5/V6 - NC
 *   V7/V8 - VIN / VCC5  (R sense = 0.01 ohms)
 * LTC2991[1] U14
 *   V1/V2 - LTM1_VOUT4/VCC_MGT_2.5V
 *   V3/V4 - LTM1_VOUT3/VCC_2.5V
 *   V5/V6 - LTM1_VOUT2/VCC_MGT_1.2V
 *   V7/V8 - LTM1_VOUT1/VCC_MGT_0.9V
 * LTC2991[2] U41
 *   V1/V2 - LTM2_VOUT4/VCC_1.2V_DDR
 *   V3/V4 - LTM2_VOUT3/VCC_1.8V
 *   V5/V6 - LTM2_VOUT1/VCC_3.3V
 *   V7/V8 - LTM3_VOUT1/VCC_0.85V
 * LTC2991[3] U61
 *   V1/V2 - ADC_AVCC_BUS/ADC_AVCC
 *   V3/V4 - ADC_AVCC_AUX_BUS/ADC_AVCC_AUX
 * LTC2991[4] U62
 *   V1/V2 - DAC_AVCC_BUS/DAC_AVCC
 *   V3/V4 - DAC_AVCC_AUX_BUS/DAC_AVCC_AUX
 *   V5/V6 - DAC AVTT_BUS/DAC_AVTT
 *
 *
 *
 *
*/


void i2c_configure_ltc2991() {


    i2c_set_port_expander(I2C_PORTEXP1_ADDR,4);
    // reg 6 (ch 1-4) & 7 (ch 5-8) control measurement type
    //   0x00 - single ended voltage measurements
    //   0x11 - measure V{1,3,5,7} s.e. and V{1,3,5,7} - V{2,4,6,8} diff
    u8 txBuf[] = {0x06, 0x00, 0x11};
    // 1-4 s.e. and 5-8 diff+s.e.
    i2c_write(txBuf, 3, 0x48);
    // all ch diff+s.e.
    txBuf[1] = 0x11;
    i2c_write(txBuf, 3, 0x49);
    i2c_write(txBuf, 3, 0x4A);
    i2c_write(txBuf, 3, 0x4B);
    i2c_write(txBuf, 3, 0x4C);

    // reg 8 bit 4 controls one-shot or continuous measurement (1 = cont)
    txBuf[0] = 0x08;
    txBuf[1] = 0x10;
    i2c_write(txBuf,2,0x48);
    i2c_write(txBuf,2,0x49);
    i2c_write(txBuf,2,0x4A);
    i2c_write(txBuf,2,0x4B);
    i2c_write(txBuf,2,0x4C);

    // reg 1 is status/control, 0xF0 enables ch 1-8 measurements
    txBuf[0] = 0x01;
    txBuf[1] = 0xF0;
    i2c_write(txBuf,2,0x48);
    i2c_write(txBuf,2,0x49);
    i2c_write(txBuf,2,0x4A);
    i2c_write(txBuf,2,0x4B);
    i2c_write(txBuf,2,0x4C);

}





s16 i2c_ltc2991_voltage(u8 i2c_addr, u8 index){
    u8 txBuf[] = {(0x0A + (2*index))};
    u8 rxBuf[2];
    s16 result;

    //txBuf[0] = 0x0A;
    //txBuf[1] = 0x0;
    i2c_write(txBuf,1,i2c_addr);
    i2c_read(rxBuf,2,i2c_addr);
    //vTaskDelay(pdMS_TO_TICKS(1));
    // MS bit is status, next is sign bit, then 14 data bits
    // mask off the status, and do an extra shift to sign-extend the result
    result = (s16)((rxBuf[0] & 0x7F) << 9) | (rxBuf[1] << 1);
    return result >> 1;
}



/*
s16 iic_ltc2991_voltage(u8 chip, u8 index){
    u8 txBuf[] = {(0x0A + (2*index))};
    u8 rxBuf[2];
    s16 result;

    txBuf[0] = 0x0A;
    txBuf[1] =
    i2c_write(txBuf,1,0x48);
    i2c_read()
    iic_chp_recv_repeated_start(txBuf, 1, rxBuf, 2, iic_ltc2991_addrs[chip]);
    iic_pe_disable(1, 2);
    // MS bit is status, next is sign bit, then 14 data bits
    // mask off the status, and do an extra shift to sign-extend the result
    result = (s16)((rxBuf[0] & 0x7F) << 9) | (rxBuf[1] << 1);
    return result >> 1;
}
*/

static const double conv_volts_se = 305.18e-6;
static const double conv_volts_diff = 19.075e-6;
static const double conv_r_sense = 0.01;

float i2c_ltc2991_reg1_temp() {
    return (0.650 - (conv_volts_se * i2c_ltc2991_voltage(0x48, 0))) / 0.002;
}

float i2c_ltc2991_reg2_temp() {
    return (0.650 - (conv_volts_se * i2c_ltc2991_voltage(0x48, 1))) / 0.002;
}

float i2c_ltc2991_reg3_temp() {
    return (0.650 - (conv_volts_se * i2c_ltc2991_voltage(0x48, 2))) / 0.002;
}

float i2c_ltc2991_reg4_temp() {
    return (0.650 - (conv_volts_se * i2c_ltc2991_voltage(0x48, 3))) / 0.002;
}



float i2c_ltc2991_vcc_vin() {
	//scale by 11 because of voltage divider on schematic
    return 11 * conv_volts_se * i2c_ltc2991_voltage(0x48, 6);
}

float i2c_ltc2991_vcc_vin_current() {
    return 11 * (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x48, 7);
}

float i2c_ltc2991_vcc_mgt_2v5() {
    return conv_volts_se * i2c_ltc2991_voltage(0x4A, 0);
}

float i2c_ltc2991_vcc_mgt_2v5_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x4A, 1);
}

float i2c_ltc2991_vcc_2v5() {
    return conv_volts_se * i2c_ltc2991_voltage(0x4A, 2);
}

float i2c_ltc2991_vcc_2v5_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x4A, 3);
}

float i2c_ltc2991_vcc_mgt_1v2() {
    return conv_volts_se * i2c_ltc2991_voltage(0x4A, 4);
}

float i2c_ltc2991_vcc_mgt_1v2_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x4A, 5);
}

float i2c_ltc2991_vcc_mgt_0v9() {
    return conv_volts_se * i2c_ltc2991_voltage(0x4A, 6);
}

float i2c_ltc2991_vcc_mgt_0v9_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x4A, 7);
}

float i2c_ltc2991_vcc_1v2_ddr() {
    return conv_volts_se * i2c_ltc2991_voltage(0x49, 0);
}

float i2c_ltc2991_vcc_1v2_ddr_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x49, 1);
}

float i2c_ltc2991_vcc_1v8() {
    return conv_volts_se * i2c_ltc2991_voltage(0x49, 2);
}

float i2c_ltc2991_vcc_1v8_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x49, 3);
}

float i2c_ltc2991_vcc_3v3() {
    return conv_volts_se * i2c_ltc2991_voltage(0x49, 4);
}

float i2c_ltc2991_vcc_3v3_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x49, 5);
}

float i2c_ltc2991_vcc_0v85() {
    return conv_volts_se * i2c_ltc2991_voltage(0x49, 6);
}

float i2c_ltc2991_vcc_0v85_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x49, 7);
}

float i2c_ltc2991_vcc_adc_avcc() {
    return conv_volts_se * i2c_ltc2991_voltage(0x4B, 0);
}

float i2c_ltc2991_vcc_adc_avcc_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x4B, 1);
}

float i2c_ltc2991_vcc_adc_avccaux() {
    return conv_volts_se * i2c_ltc2991_voltage(0x4B, 2);
}

float i2c_ltc2991_vcc_adc_avccaux_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x4B, 3);
}

float i2c_ltc2991_vcc_dac_avcc() {
    return conv_volts_se * i2c_ltc2991_voltage(0x4C, 0);
}

float i2c_ltc2991_vcc_dac_avcc_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x4C, 1);
}

float i2c_ltc2991_vcc_dac_avccaux() {
    return conv_volts_se * i2c_ltc2991_voltage(0x4C, 2);
}

float i2c_ltc2991_vcc_dac_avccaux_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x4C, 3);
}

float i2c_ltc2991_vcc_dac_avtt() {
    return conv_volts_se * i2c_ltc2991_voltage(0x4C, 4);
}

float i2c_ltc2991_vcc_dac_avtt_current() {
    return (conv_volts_diff / conv_r_sense) * i2c_ltc2991_voltage(0x4C, 5);
}



/*
u16 iic_ltc2991_read16(u8 index, u8 addr){
    iic_pe_enable(1,2);
    u8 txBuf[] = {addr};
    u8 rxBuf[2];
    iic_chp_recv_repeated_start(txBuf, 1, rxBuf, 2, iic_ltc2991_addrs[index]);
    u16 temp = (rxBuf[0] << 8) + rxBuf[1];
    iic_pe_disable(1,2);
    return temp;
}
u8 iic_ltc2991_read8(u8 index, u8 addr){
    iic_pe_enable(1,2);
    u8 txBuf[] = {addr};
    u8 rxBuf[1];
    iic_chp_recv_repeated_start(txBuf, 1, rxBuf, 1, iic_ltc2991_addrs[index]);
    iic_pe_disable(1,2);
    return rxBuf[0];
}
void iic_ltc2991_write16(u8 index, u8 addr, u16 data){
    iic_pe_enable(1, 2);
    u8 txBuf[] = {addr, (data >> 8), (data & 0xFF)};
    iic_chp_send(txBuf, 3, iic_ltc2991_addrs[index]);
    iic_pe_disable(1, 2);
}
void iic_ltc2991_write8(u8 index, u8 addr, u8 data){
    iic_pe_enable(1, 2);
    u8 txBuf[] = {addr, data};
    iic_chp_send(txBuf, 2, iic_ltc2991_addrs[index]);
    iic_pe_disable(1, 2);
}
*/



