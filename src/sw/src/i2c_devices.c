#include "xparameters.h"
#include "xiicps.h"
#include "rfsoc-dfe_defs.h"
#include <sleep.h>
#include "xil_printf.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"


extern XIicPs IicPsInstance;			/* Instance of the IIC Device */


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

/*
void i2c_sfp_get_stats(struct SysHealthStatsMsg *p, u8 sfp_slot) {

    u8 addr = 0x51;  //SFP A2 address space
    u8 buf[10];
    u32 temp;
    float tempflt;

    buf[0] = 96;  //offset location


	i2c_set_port_expander(I2C_PORTEXP0_ADDR,1);
	i2c_set_port_expander(I2C_PORTEXP1_ADDR,0);
	//read 10 bytes starting at address 96 (see data sheet)
    i2c_write(buf,1,addr);
    i2c_read(buf,10,addr);
    temp = (buf[0] << 8) | (buf[1]);
    p->sfp_temp[0] = (float)temp/256.0;
    printf("SFP Temp = %f\r\n", p->sfp_temp[sfp_slot]);

    temp = (buf[2] << 8) | (buf[3]);
    tempflt = (float)temp/10000.0;

    printf("SFP VCC = %f\r\n", tempflt);
    temp = (buf[4] << 8) | (buf[5]);
    tempflt = (float)temp/200.0;
    printf("SFP Tx Laser Bias = %f\r\n", tempflt);
    temp = (buf[6] << 8) | (buf[7]);
    tempflt = (float)temp/10000.0;
    printf("SFP Tx Pwr = %f\r\n", tempflt);
    temp = (buf[8] << 8) | (buf[9]);
    tempflt = (float)temp/10000.0;
    printf("SFP Rx Pwr = %f\r\n", tempflt);

}
*/

void i2c_get_ltc2991()
{
  printf("LTC2991 Voltage/Current/Temperature Monitoring\r\n");
  printf("----\r\n");
  printf("V 12.0:      %0.2f\r\n", i2c_ltc2991_vcc_vin());
  printf("I 12.0:      %0.2f\r\n", i2c_ltc2991_vcc_vin_current());
  printf("V 3.3:      %0.2f\r\n", i2c_ltc2991_vcc_3v3());
  printf("I 3.3:      %0.2f\r\n", i2c_ltc2991_vcc_3v3_current());
  printf("V 2.5:      %0.2f\r\n", i2c_ltc2991_vcc_2v5());
  printf("I 2.5:      %0.2f\r\n", i2c_ltc2991_vcc_2v5_current());
  printf("V 1.8:      %0.2f\r\n", i2c_ltc2991_vcc_1v8());
  printf("I 1.8:      %0.2f\r\n", i2c_ltc2991_vcc_1v8_current());
  printf("V 0.85:     %0.2f\r\n", i2c_ltc2991_vcc_0v85());
  printf("I 0.85:     %0.2f\r\n", i2c_ltc2991_vcc_0v85_current());
  printf("V 2.5 MGT:  %0.2f\r\n", i2c_ltc2991_vcc_mgt_2v5());
  printf("I 2.5 MGT:  %0.2f\r\n", i2c_ltc2991_vcc_mgt_2v5_current());
  printf("V 1.2 MGT:  %0.2f\r\n", i2c_ltc2991_vcc_mgt_1v2());
  printf("I 1.2 MGT:  %0.2f\r\n", i2c_ltc2991_vcc_mgt_1v2_current());
  printf("V 0.9 MGT:  %0.2f\r\n", i2c_ltc2991_vcc_mgt_0v9());
  printf("I 0.9 MGT:  %0.2f\r\n", i2c_ltc2991_vcc_mgt_0v9_current());
  printf("V 1.2 DDR:  %0.2f\r\n", i2c_ltc2991_vcc_1v2_ddr());
  printf("I 1.2 DDR:  %0.2f\r\n", i2c_ltc2991_vcc_1v2_ddr_current());

  printf("reg1_temp:  %0.2f\r\n", i2c_ltc2991_reg1_temp());
  printf("reg2_temp:  %0.2f\r\n", i2c_ltc2991_reg2_temp());
  printf("reg3_temp:  %0.2f\r\n", i2c_ltc2991_reg3_temp());
  printf("----\r\n");
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
 * LTC2991[2] U37
 *   V1/V2 - LTM2_VOUT4/VCC_1.2V_DDR
 *   V3/V4 - LTM2_VOUT3/VCC_1.8V
 *   V5/V6 - LTM2_VOUT1/VCC_3.3V
 *   V7/V8 - LTM3_VOUT1/VCC_0.85V
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

    // reg 8 bit 4 controls one-shot or continuous measurement (1 = cont)
    txBuf[0] = 0x08;
    txBuf[1] = 0x10;
    i2c_write(txBuf,2,0x48);
    i2c_write(txBuf,2,0x49);
    i2c_write(txBuf,2,0x4A);

    // reg 1 is status/control, 0xF0 enables ch 1-8 measurements
    txBuf[0] = 0x01;
    txBuf[1] = 0xF0;
    i2c_write(txBuf,2,0x48);
    i2c_write(txBuf,2,0x49);
    i2c_write(txBuf,2,0x4A);

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

