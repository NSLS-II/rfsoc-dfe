
library IEEE;
use IEEE.std_logic_1164.ALL;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;


  
package bpm_package is

type t_adc_raw is array(0 to 3) of std_logic_vector(15 downto 0);

type sfp_i2c_data_type is array(0 to 5) of std_logic_vector(15 downto 0);



type t_reg_i_rfadc_fifo_rdout is record
   adc0_dout    : std_logic_vector(31 downto 0);
   adc0_rdcnt   : std_logic_vector(31 downto 0);    
   adc1_dout    : std_logic_vector(31 downto 0); 
   adc1_rdcnt   : std_logic_vector(31 downto 0);      
   adc2_dout    : std_logic_vector(31 downto 0);
   adc2_rdcnt   : std_logic_vector(31 downto 0); 
   adc3_dout    : std_logic_vector(31 downto 0);     
   adc3_rdcnt   : std_logic_vector(31 downto 0);  
end record t_reg_i_rfadc_fifo_rdout;


type t_reg_o_rfadc_fifo_rdout is record
   enb          : std_logic;
   rst          : std_logic;
   adc0_rdstr   : std_logic;
   adc1_rdstr   : std_logic;
   adc2_rdstr   : std_logic;
   adc3_rdstr   : std_logic;         
end record t_reg_o_rfadc_fifo_rdout;




type t_reg_i_adc_fifo_rdout is record
   dout     : std_logic_vector(31 downto 0);
   rdcnt    : std_logic_vector(31 downto 0); 
end record t_reg_i_adc_fifo_rdout;

type t_reg_o_adc_fifo_rdout is record
   enb      : std_logic;
   rst      : std_logic;
   rdstr    : std_logic;
end record t_reg_o_adc_fifo_rdout;


type t_reg_i_tbt_fifo_rdout is record
   dout     : std_logic_vector(31 downto 0);
   rdcnt    : std_logic_vector(31 downto 0); 
end record t_reg_i_tbt_fifo_rdout;

type t_reg_o_tbt_fifo_rdout is record
   enb      : std_logic;
   rst      : std_logic;
   rdstr    : std_logic;
end record t_reg_o_tbt_fifo_rdout;


type t_reg_o_dsa is record
   str      : std_logic;
   data     : std_logic_vector(7 downto 0);
end record t_reg_o_dsa;

type t_reg_o_evr is record
   reset         : std_logic;
   dma_trigno    : std_logic_vector(7 downto 0);
   event_src_sel : std_logic;
end record t_reg_o_evr;

type t_reg_i_evr is record
   ts_ns      : std_logic_vector(31 downto 0);
   ts_s       : std_logic_vector(31 downto 0);
end record t_reg_i_evr;


type t_reg_o_pll is record
   str      : std_logic;
   data     : std_logic_vector(31 downto 0);
end record t_reg_o_pll;

type t_reg_i_pll is record
   locked      : std_logic;
end record t_reg_i_pll;


type t_reg_o_therm is record
   spi_we     : std_logic;
   spi_wdata  : std_logic_vector(31 downto 0);
   sel        : std_logic_vector(1 downto 0);
end record t_reg_o_therm;

type t_reg_i_therm is record
   spi_rdata    : std_logic_vector(7 downto 0);
end record t_reg_i_therm;



type t_tbt_data is record
    cha_mag    : signed(31 downto 0);
    cha_phs    : signed(31 downto 0);
    cha_i      : signed(31 downto 0);
    cha_q      : signed(31 downto 0);
    chb_mag    : signed(31 downto 0);
    chb_phs    : signed(31 downto 0); 
    chb_i      : signed(31 downto 0);
    chb_q      : signed(31 downto 0);
    chc_mag    : signed(31 downto 0);
    chc_phs    : signed(31 downto 0); 
    chc_i      : signed(31 downto 0);
    chc_q      : signed(31 downto 0);
    chd_mag    : signed(31 downto 0);
    chd_phs    : signed(31 downto 0);     
    chd_i      : signed(31 downto 0);
    chd_q      : signed(31 downto 0);
    xpos       : signed(31 downto 0);
    ypos       : signed(31 downto 0);
    xpos_nm    : signed(31 downto 0);
    ypos_nm    : signed(31 downto 0);
    sum        : signed(31 downto 0);
end record t_tbt_data;


type t_sa_data is record
    cnt        : std_logic_vector(31 downto 0);
    cha_mag    : signed(31 downto 0);
    chb_mag    : signed(31 downto 0);
    chc_mag    : signed(31 downto 0);
    chd_mag    : signed(31 downto 0);     
    xpos       : signed(31 downto 0);
    ypos       : signed(31 downto 0);
    sum        : signed(31 downto 0);
end record t_sa_data;


type t_fa_data is record
    cha_mag    : signed(31 downto 0);
    chb_mag    : signed(31 downto 0);
    chc_mag    : signed(31 downto 0); 
    chd_mag    : signed(31 downto 0);     
    xpos       : signed(31 downto 0);
    ypos       : signed(31 downto 0);
    sum        : signed(31 downto 0);
end record t_fa_data;


type rffe_sw_params_type is record
    adcdma_sel   : std_logic;
    enb          : std_logic_vector(1 downto 0);
    demuxdly     : std_logic_vector(8 downto 0);
    trigdly      : std_logic_vector(15 downto 0);
end record rffe_sw_params_type;


type t_reg_o_tbt is record
    kx          : std_logic_vector(31 downto 0);
    ky          : std_logic_vector(31 downto 0);
    cha_gain    : std_logic_vector(15 downto 0);
    chb_gain    : std_logic_vector(15 downto 0);
    chc_gain    : std_logic_vector(15 downto 0);
    chd_gain    : std_logic_vector(15 downto 0);      
    xpos_offset : std_logic_vector(31 downto 0);
    ypos_offset : std_logic_vector(31 downto 0); 
    gate_delay  : std_logic_vector(8 downto 0); 
    gate_width  : std_logic_vector(8 downto 0);
end record t_reg_o_tbt;


type t_reg_o_dma is record
    soft_trig    : std_logic;
    trigsrc      : std_logic;
    testdata_enb : std_logic;
    adc_len      : std_logic_vector(31 downto 0);
    tbt_len      : std_logic_vector(31 downto 0);
    fa_len       : std_logic_vector(31 downto 0);
    fifo_rst     : std_logic;
    adc_enb      : std_logic;
    tbt_enb      : std_logic;
    fa_enb       : std_logic;
end record t_reg_o_dma;

type t_reg_i_dma is record
    trig_cnt     : std_logic_vector(31 downto 0); 
    status       : std_logic_vector(4 downto 0);
    ts_s         : std_logic_vector(31 downto 0);
    ts_ns        : std_logic_vector(31 downto 0);
end record t_reg_i_dma;







component system is
  port (
    adc2_clk_clk_n : in STD_LOGIC;
    adc2_clk_clk_p : in STD_LOGIC;
    
    vin2_01_v_n : in STD_LOGIC;
    vin2_01_v_p : in STD_LOGIC;
    vin2_23_v_n : in STD_LOGIC;
    vin2_23_v_p : in STD_LOGIC;  
    vin3_01_v_n : in STD_LOGIC;
    vin3_01_v_p : in STD_LOGIC;
    vin3_23_v_n : in STD_LOGIC;
    vin3_23_v_p : in STD_LOGIC;    
    
    clk_adc2_0 : out STD_LOGIC;
    clk_adc3_0 : out STD_LOGIC;
    
    m20_axis_0_tdata : out STD_LOGIC_VECTOR ( 191 downto 0 );
    m20_axis_0_tready : in STD_LOGIC;
    m20_axis_0_tvalid : out STD_LOGIC;
    m22_axis_0_tdata : out STD_LOGIC_VECTOR ( 191 downto 0 );
    m22_axis_0_tready : in STD_LOGIC;
    m22_axis_0_tvalid : out STD_LOGIC;    
    m30_axis_0_tdata : out STD_LOGIC_VECTOR ( 191 downto 0 );
    m30_axis_0_tready : in STD_LOGIC;
    m30_axis_0_tvalid : out STD_LOGIC;
    m32_axis_0_tdata : out STD_LOGIC_VECTOR ( 191 downto 0 );
    m32_axis_0_tready : in STD_LOGIC;
    m32_axis_0_tvalid : out STD_LOGIC;    
 
    m2_axis_aclk_0 : in STD_LOGIC;
    m3_axis_aclk_0 : in STD_LOGIC;
    m2_axis_aresetn_0 : in STD_LOGIC;
    m3_axis_aresetn_0 : in STD_LOGIC;
    
    m_axi_araddr : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_arprot : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_arready : in STD_LOGIC;
    m_axi_arvalid : out STD_LOGIC;
    m_axi_awaddr : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_awprot : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_awready : in STD_LOGIC;
    m_axi_awvalid : out STD_LOGIC;
    m_axi_bready : out STD_LOGIC;
    m_axi_bresp : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_bvalid : in STD_LOGIC;
    m_axi_rdata : in STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_rready : out STD_LOGIC;
    m_axi_rresp : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_rvalid : in STD_LOGIC;
    m_axi_wdata : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_wready : in STD_LOGIC;
    m_axi_wstrb : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_wvalid : out STD_LOGIC;
    pl_clk0 : out STD_LOGIC;
    pl_resetn : out STD_LOGIC;
    sysref_in_diff_n : in STD_LOGIC;
    sysref_in_diff_p : in STD_LOGIC

  );
end component system;





	


end bpm_package;
  
