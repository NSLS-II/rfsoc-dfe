

#include <metal/log.h>
#include <metal/sys.h>


#define BRDTEMP0_ADDR 0x48
#define BRDTEMP1_ADDR 0x49
#define BRDTEMP2_ADDR 0x4A
#define BRDTEMP3_ADDR 0x4B

#define I2C_PORTEXP0_ADDR 0x70
#define I2C_PORTEXP1_ADDR 0x71
#define I2C_PORTEXP2_ADDR 0x72



void my_metal_default_log_handler(enum metal_log_level level,
			       const char *format, ...);


u32 InitRFdc(void);
void GetRFdc_Status(void);
void WriteLMK04828(void);
void prog_ad9510(unsigned int *);
void ltc2195_init(unsigned int *);

void psc_control_thread();
void psc_status_thread();
void psc_wvfm_thread();

void read_board_temps();
void init_i2c();
s32 i2c_read(u8 *, u8, u8);
s32 i2c_write(u8 *, u8, u8);
void i2c_set_port_expander(u32, u32);
float read_i2c_temp(u8);

float L11_to_float(s32);

void i2c_configure_ltc2991();
void i2c_get_ltc2991();
float i2c_ltc2991_reg1_temp();
float i2c_ltc2991_reg2_temp();
float i2c_ltc2991_reg3_temp();
float i2c_ltc2991_vcc_vin();
float i2c_ltc2991_vcc_vin_current();
float i2c_ltc2991_vcc_mgt_2v5();
float i2c_ltc2991_vcc_mgt_2v5_current();
float i2c_ltc2991_vcc_2v5();
float i2c_ltc2991_vcc_2v5_current();
float i2c_ltc2991_vcc_mgt_1v2();
float i2c_ltc2991_vcc_mgt_1v2_current();
float i2c_ltc2991_vcc_mgt_0v9();
float i2c_ltc2991_vcc_mgt_0v9_current();
float i2c_ltc2991_vcc_1v2_ddr();
float i2c_ltc2991_vcc_1v2_ddr_current();
float i2c_ltc2991_vcc_1v8();
float i2c_ltc2991_vcc_1v8_current();
float i2c_ltc2991_vcc_3v3();
float i2c_ltc2991_vcc_3v3_current();
float i2c_ltc2991_vcc_0v85();
float i2c_ltc2991_vcc_0v85_current();

//void i2c_sfp_get_stats(struct *, u8);




