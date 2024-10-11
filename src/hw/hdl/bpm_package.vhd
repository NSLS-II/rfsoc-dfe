
library IEEE;
use IEEE.std_logic_1164.ALL;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;


  
package bpm_package is

type adc_raw_type is array(0 to 3) of std_logic_vector(15 downto 0);

type sfp_i2c_data_type is array(0 to 5) of std_logic_vector(15 downto 0);


type sdi_cntrl_type is record
   reset       : std_logic_vector(15 downto 0);
   my_addr     : std_logic_vector(15 downto 0);
   stop_addr   : std_logic_vector(15 downto 0);
   fa_trig_dly : std_logic_vector(11 downto 0);
   
end record sdi_cntrl_type;

type sdi_status_type is record
   cw_crcerrcnt          : std_logic_vector(31 downto 0);
   ccw_crcerrcnt         : std_logic_vector(31 downto 0);
   fa_trig_cnt           : std_logic_vector(31 downto 0);
   fa_trig_sync_cnt      : std_logic_vector(31 downto 0);
   cw_timeout_cnt_fault  : std_logic_vector(15 downto 0);
   ccw_timeout_cnt_fault : std_logic_vector(15 downto 0);
end record sdi_status_type;

type brd_temps_type is record
   temp0 : std_logic_vector(15 downto 0);
   temp1 : std_logic_vector(15 downto 0);
   temp2 : std_logic_vector(15 downto 0);
   temp3 : std_logic_vector(15 downto 0);
end record brd_temps_type;


type afe_regs_type is record
   temp0 : std_logic_vector(15 downto 0);
   temp1 : std_logic_vector(15 downto 0);
   temp2 : std_logic_vector(15 downto 0);
   temp3 : std_logic_vector(15 downto 0);
   Vreg0 : std_logic_vector(15 downto 0);
   Vreg1 : std_logic_vector(15 downto 0);
   Vreg2 : std_logic_vector(15 downto 0);
   Vreg3 : std_logic_vector(15 downto 0);
   Vreg4 : std_logic_vector(15 downto 0);
   Vreg5 : std_logic_vector(15 downto 0);
   Vreg6 : std_logic_vector(15 downto 0);
   Vreg7 : std_logic_vector(15 downto 0);
   Ireg0 : std_logic_vector(15 downto 0);
   Ireg1 : std_logic_vector(15 downto 0);
   Ireg2 : std_logic_vector(15 downto 0);
   Ireg3 : std_logic_vector(15 downto 0);
   Ireg4 : std_logic_vector(15 downto 0);
   Ireg5 : std_logic_vector(15 downto 0);
   Ireg6 : std_logic_vector(15 downto 0);
   Ireg7 : std_logic_vector(15 downto 0);
end record afe_regs_type;




type tbt_data_type is record
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
end record tbt_data_type;


type sa_data_type is record
    cha_mag    : signed(31 downto 0);
    chb_mag    : signed(31 downto 0);
    chc_mag    : signed(31 downto 0);
    chd_mag    : signed(31 downto 0);     
    xpos       : signed(31 downto 0);
    ypos       : signed(31 downto 0);
    sum        : signed(31 downto 0);
end record sa_data_type;


type fa_data_type is record
    cha_mag    : signed(31 downto 0);
    chb_mag    : signed(31 downto 0);
    chc_mag    : signed(31 downto 0); 
    chd_mag    : signed(31 downto 0);     
    xpos       : signed(31 downto 0);
    ypos       : signed(31 downto 0);
    sum        : signed(31 downto 0);
end record fa_data_type;


type rffe_sw_params_type is record
    adcdma_sel   : std_logic;
    enb          : std_logic_vector(1 downto 0);
    demuxdly     : std_logic_vector(8 downto 0);
    trigdly      : std_logic_vector(15 downto 0);
end record rffe_sw_params_type;


type tbt_params_type is record
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
end record tbt_params_type;


type dma_params_type is record
    dma_adc_length   : std_logic_vector(31 downto 0);
    dma_tbt_length   : std_logic_vector(31 downto 0);
    dma_fifo_rst     : std_logic;
    dma_adc_enb      : std_logic;
    dma_tbt_enb      : std_logic;
    dma_testdata_enb : std_logic;
end record dma_params_type;



component system is
  port (
    pl_clk0 : out STD_LOGIC;
    pl_resetn : out STD_LOGIC;
    m_axi_awaddr : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_awprot : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_awvalid : out STD_LOGIC;
    m_axi_awready : in STD_LOGIC;
    m_axi_wdata : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_wstrb : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_wvalid : out STD_LOGIC;
    m_axi_wready : in STD_LOGIC;
    m_axi_bresp : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_bvalid : in STD_LOGIC;
    m_axi_bready : out STD_LOGIC;
    m_axi_araddr : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_arprot : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_arvalid : out STD_LOGIC;
    m_axi_arready : in STD_LOGIC;
    m_axi_rdata : in STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_rresp : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_rvalid : in STD_LOGIC;
    m_axi_rready : out STD_LOGIC
  );
  end component system;





	


end bpm_package;
  
