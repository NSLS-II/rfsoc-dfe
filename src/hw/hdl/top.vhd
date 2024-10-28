
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;

library desyrdl;
use desyrdl.common.all;

use desyrdl.pkg_pl_regs.all;

library xil_defaultlib;
use xil_defaultlib.bpm_package.ALL;


entity top is
generic(
    FPGA_VERSION			: integer := 9;
    SIM_MODE				: integer := 0
    );
  port (  
   
    clk104_lmkin0_clk_p     : out std_logic; -- clk104 lmk04828 clkin0
    clk104_lmkin0_clk_n     : out std_logic;
    clk104_adc_refclk_p     : in std_logic;  -- clk104 lmk04828 Dout12
    clk104_adc_refclk_n     : in std_logic; 
    clk104_dac_refclk_p     : in std_logic;  -- clk104 lmk04828 Dout6
    clk104_dac_refclk_n     : in std_logic;
    clk104_pl_clk_p         : in std_logic;
    clk104_pl_clk_n         : in std_logic;
    clk104_pl_sysref_p      : in std_logic; -- clk104 AMS sysref SDOut3
    clk104_pl_sysref_n      : in std_logic;   
    
    adc0_in_p               : in std_logic; 
    adc0_in_n               : in std_logic;  
  
  
    fp_led                  : out std_logic_vector(7 downto 0);
    dbg                     : out std_logic_vector(19 downto 0)

  );
end top;


architecture behv of top is

  
  signal pl_clk0      : std_logic;
  signal pl_resetn    : std_logic;
  signal pl_reset     : std_logic;
  
  signal m_axi4_m2s : t_pl_regs_m2s;
  signal m_axi4_s2m : t_pl_regs_s2m;
  
  signal reg_i      : t_addrmap_pl_regs_in;
  signal reg_o      : t_addrmap_pl_regs_out;
  
  signal clk104_pl_clkin : std_logic;
  signal clk104_pl_clk   : std_logic;
  
  signal adc0_axis_tdata        : std_logic_vector(191 downto 0); 
  signal adc0_axis_tready       : std_logic;
  signal adc0_axis_tvalid       : std_logic; 
  signal rfadc_out_clk          : std_logic;
  signal rfadc_axis_mmcm_clk    : std_logic;
  signal rfadc_axis_clk         : std_logic;
  
  
  attribute mark_debug     : string;
  attribute mark_debug of adc0_axis_tdata: signal is "true"; 
  attribute mark_debug of adc0_axis_tvalid: signal is "true"; 
  attribute mark_debug of pl_reset: signal is "true";

  --attribute mark_debug of reg_o: signal is "true";
  --attribute mark_debug of reg_i: signal is "true";

begin


dbg(0) <= pl_clk0;
dbg(1) <= '0';
dbg(2) <= clk104_pl_clk;
dbg(3) <= '0';
dbg(4) <= rfadc_out_clk;
dbg(5) <= '0';
dbg(6) <= rfadc_axis_clk;
dbg(19 downto 7) <= (others => '0'); 

pl_reset <= not pl_resetn;

--drive the CLK104 PLL with 100MHz for now
lmk_clkout : OBUFDS port map (O => clk104_lmkin0_clk_p, OB => clk104_lmkin0_clk_n, I => pl_clk0);   


lmk_pl_clkin  : IBUFDS port map (O => clk104_pl_clkin, I => clk104_pl_clk_p, IB => clk104_pl_clk_n);
pl_clkin_bufg : BUFG   port map (O => clk104_pl_clk, I => clk104_pl_clkin);

rfadc_bufg    : BUFG   port map (O => rfadc_axis_clk, I => rfadc_axis_mmcm_clk);



axisclk_adc: entity work.rfadc_clk_pll  
  port map (
    reset => not pl_resetn, 
    clk_in1 => rfadc_out_clk, 
    clk_out1 => rfadc_axis_mmcm_clk, 
    locked => open  --adc_axis_pll_locked  
);



fp_led <= reg_o.fp_leds.val.data;
--fp_led(7 downto 0) <= "01010101"; --gpio_leds_i(5 downto 0);

regs: pl_regs
  port map (
    pi_clock => pl_clk0, 
    pi_reset => not pl_resetn, 
    -- TOP subordinate memory mapped interface
    --pi_s_reset => '0', 
    pi_s_top => m_axi4_m2s, 
    po_s_top => m_axi4_s2m, 
    -- to logic interface
    pi_addrmap => reg_i,  
    po_addrmap => reg_o
  );

  

store_adc0:  entity work.adc_data_rdout
  port map (
    sys_clk => pl_clk0, 
    adc_clk => rfadc_axis_clk,  
    sys_rst => pl_reset,
    adc_data => adc0_axis_tdata,
    fifo_trig => reg_o.adc0fifo_trig.data.data(0),  
    fifo_rdstr => reg_o.adc0fifo_dout.data.swacc, 
    fifo_dout => reg_i.adc0fifo_dout.data.data,  
    fifo_rdcnt => reg_i.adc0fifo_wdcnt.data.data, 
    fifo_rst => reg_o.adc0fifo_reset.data.data(0)
 );







system_i: component system
  port map (
    pl_clk0 => pl_clk0,
    pl_resetn => pl_resetn,
     
    m_axi_araddr => m_axi4_m2s.araddr, 
    m_axi_arprot => m_axi4_m2s.arprot,
    m_axi_arready => m_axi4_s2m.arready,
    m_axi_arvalid => m_axi4_m2s.arvalid,
    m_axi_awaddr => m_axi4_m2s.awaddr,
    m_axi_awprot => m_axi4_m2s.awprot,
    m_axi_awready => m_axi4_s2m.awready,
    m_axi_awvalid => m_axi4_m2s.awvalid,
    m_axi_bready => m_axi4_m2s.bready,
    m_axi_bresp => m_axi4_s2m.bresp,
    m_axi_bvalid => m_axi4_s2m.bvalid,
    m_axi_rdata => m_axi4_s2m.rdata,
    m_axi_rready => m_axi4_m2s.rready,
    m_axi_rresp => m_axi4_s2m.rresp,
    m_axi_rvalid => m_axi4_s2m.rvalid,
    m_axi_wdata => m_axi4_m2s.wdata,
    m_axi_wready => m_axi4_s2m.wready,
    m_axi_wstrb => m_axi4_m2s.wstrb,
    m_axi_wvalid => m_axi4_m2s.wvalid,
    
    
    adc2_clk_clk_p => clk104_adc_refclk_p,
    adc2_clk_clk_n => clk104_adc_refclk_n,
    sysref_in_diff_p => clk104_pl_sysref_p,
    sysref_in_diff_n => clk104_pl_sysref_n,
    vin2_01_v_p => adc0_in_p,
    vin2_01_v_n => adc0_in_n, 
    m2_axis_aclk_0 => rfadc_axis_clk,
    m2_axis_aresetn_0 => pl_resetn,    
    m20_axis_0_tready => '1',    
    clk_adc2_0 => rfadc_out_clk, 
    m20_axis_0_tdata => adc0_axis_tdata, 
    m20_axis_0_tvalid => adc0_axis_tvalid

    );



end behv;
