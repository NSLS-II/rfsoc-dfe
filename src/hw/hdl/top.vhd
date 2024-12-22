
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
    adc1_in_p               : in std_logic; 
    adc1_in_n               : in std_logic;      
    adc2_in_p               : in std_logic; 
    adc2_in_n               : in std_logic;  
    adc3_in_p               : in std_logic; 
    adc3_in_n               : in std_logic;  
    
    -- evr
    gty_evr_refclk_p        : in std_logic; 
    gty_evr_refclk_n        : in std_logic; 
    gty_evr_tx_p            : out std_logic; 
    gty_evr_tx_n            : out std_logic;
    gty_evr_rx_p            : in std_logic; 
    gty_evr_rx_n            : in std_logic; 
    
    sfp_led                 : out std_logic_vector(11 downto 0);
    sfp_rxlos               : in std_logic_vector(5 downto 0);  
    fp_in                   : in std_logic_vector(3 downto 0);
    fp_out                  : out std_logic_vector(3 downto 0);    
       
    fp_led                  : out std_logic_vector(7 downto 0);
    dbg                     : out std_logic_vector(19 downto 0)

  );
end top;


architecture behv of top is

  
  signal pl_clk0      : std_logic;
  signal pl_resetn    : std_logic;
  signal pl_reset     : std_logic;
  signal ps_leds      : std_logic_vector(7 downto 0);
  
  signal m_axi4_m2s : t_pl_regs_m2s;
  signal m_axi4_s2m : t_pl_regs_s2m;
  
  signal reg_i      : t_addrmap_pl_regs_in;
  signal reg_o      : t_addrmap_pl_regs_out;
  
  signal clk104_pl_clkin : std_logic;
  signal clk104_pl_clk   : std_logic;
  
  signal adc_data        : t_adc_raw; 
  
  signal adc0_axis_tdata        : std_logic_vector(191 downto 0); 
  signal adc0_axis_tready       : std_logic;
  signal adc0_axis_tvalid       : std_logic; 
  signal adc1_axis_tdata        : std_logic_vector(191 downto 0); 
  signal adc1_axis_tready       : std_logic;
  signal adc1_axis_tvalid       : std_logic; 
  signal adc2_axis_tdata        : std_logic_vector(191 downto 0); 
  signal adc2_axis_tready       : std_logic;
  signal adc2_axis_tvalid       : std_logic;   
  signal adc3_axis_tdata        : std_logic_vector(191 downto 0); 
  signal adc3_axis_tready       : std_logic;
  signal adc3_axis_tvalid       : std_logic;     
  
  signal rfadc_out_clk          : std_logic;
  signal rfadc_axis_mmcm_clk    : std_logic;
  signal rfadc_axis_clk         : std_logic;
  
  signal reg_o_adcfifo   : t_reg_o_adc_fifo_rdout;
  signal reg_i_adcfifo   : t_reg_i_adc_fifo_rdout;
  signal reg_o_tbtfifo   : t_reg_o_tbt_fifo_rdout;
  signal reg_i_tbtfifo   : t_reg_i_tbt_fifo_rdout;

  
  signal reg_o_dsa       : t_reg_o_dsa;  
  signal reg_o_pll       : t_reg_o_pll;
  signal reg_i_pll       : t_reg_i_pll;
  signal reg_i_rfadcfifo : t_reg_i_rfadc_fifo_rdout;
  signal reg_o_rfadcfifo : t_reg_o_rfadc_fifo_rdout;	 
  
  signal reg_o_tbt       : t_reg_o_tbt;
  signal reg_o_dma       : t_reg_o_dma;
  signal reg_i_dma       : t_reg_i_dma;
  signal reg_o_evr       : t_reg_o_evr;
  signal reg_i_evr       : t_reg_i_evr;
  signal reg_o_therm     : t_reg_o_therm;
  signal reg_i_therm     : t_reg_i_therm;
  
  signal tbt_data        : t_tbt_data;    
  signal sa_data         : t_sa_data;
  signal fa_data         : t_fa_data;
  
  signal evr_gty_reset       : std_logic_vector(7 downto 0);
  signal evr_ref_clk         : std_logic;
  signal evr_rcvd_clk        : std_logic;
  signal evr_tbt_trig        : std_logic;
  signal evr_fa_trig         : std_logic;
  signal evr_sa_trig         : std_logic;
  signal evr_gps_trig        : std_logic;
  signal evr_dma_trig        : std_logic;
  signal evr_dbg             : std_logic_vector(19 downto 0);
  signal evr_dma_trignum     : std_logic_vector(7 downto 0);
  signal evr_ts              : std_logic_vector(63 downto 0); 
  
  
  attribute mark_debug     : string;
  attribute mark_debug of adc0_axis_tdata: signal is "true"; 
  attribute mark_debug of adc0_axis_tvalid: signal is "true"; 
  attribute mark_debug of pl_reset: signal is "true";
  attribute mark_debug of fp_led: signal is "true";

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


sfp_led(1 downto 0) <= sfp_rxlos(0) & ps_leds(0);
sfp_led(3 downto 2) <= sfp_rxlos(1) & ps_leds(1);
sfp_led(5 downto 4) <= sfp_rxlos(2) & ps_leds(2);
sfp_led(7 downto 6) <= sfp_rxlos(3) & ps_leds(3);
sfp_led(9 downto 8) <= sfp_rxlos(4) & ps_leds(4);
sfp_led(11 downto 10) <= sfp_rxlos(5) & ps_leds(5);

fp_out(0) <= fp_led(0); --clk104_pl_clk;
fp_out(1) <= fp_led(1); --evr_rcvd_clk;
fp_out(2) <= fp_led(2); --rfadc_axis_clk;  --125 MHz
fp_out(3) <= fp_led(3); --rfadc_axis_mmcm_clk; -- 166MHz


fp_led  <= ps_leds;



pl_reset <= not pl_resetn;

--drive the CLK104 PLL with 100MHz for now
lmk_clkout : OBUFDS port map (O => clk104_lmkin0_clk_p, OB => clk104_lmkin0_clk_n, I => pl_clk0);   


lmk_pl_clkin  : IBUFDS port map (O => clk104_pl_clkin, I => clk104_pl_clk_p, IB => clk104_pl_clk_n);
pl_clkin_bufg : BUFG   port map (O => clk104_pl_clk, I => clk104_pl_clkin);

rfadc_bufg    : BUFG   port map (O => rfadc_axis_clk, I => rfadc_axis_mmcm_clk);



axisclk_adc: entity work.rfadc_clk_pll  
  port map (
    reset => not pl_resetn, 
    clk_in1 => rfadc_out_clk,   --125MHz
    clk_out1 => rfadc_axis_mmcm_clk, 
    locked => open  --adc_axis_pll_locked  
);




ps_pl: entity work.ps_io
  port map (
    pl_clock => pl_clk0, 
    pl_reset => not pl_resetn, 
    m_axi4_m2s => m_axi4_m2s, 
    m_axi4_s2m => m_axi4_s2m, 
    fp_leds => ps_leds,
    adc_data => adc_data,
    sa_data => sa_data,
    reg_o_tbt => reg_o_tbt,  
    reg_i_rfadcfifo => reg_i_rfadcfifo, 
    reg_o_rfadcfifo => reg_o_rfadcfifo, 	 
    reg_o_adcfifo => reg_o_adcfifo, 
	reg_i_adcfifo => reg_i_adcfifo,
	reg_o_tbtfifo => reg_o_tbtfifo, 
	reg_i_tbtfifo => reg_i_tbtfifo,
	reg_o_dma => reg_o_dma,
	reg_i_dma => reg_i_dma,
	reg_o_dsa => reg_o_dsa,
	reg_o_pll => reg_o_pll,
	reg_i_pll => reg_i_pll,
	reg_o_evr => reg_o_evr, 
	reg_i_evr => reg_i_evr
          
  );


  
  
rfadc_fifos:  entity work.rf_adc_fifos
  port map (
    pl_clk0 => pl_clk0,  
    pl_reset => pl_reset, 
    adc_clk => rfadc_axis_clk,  
    reg_i => reg_i_rfadcfifo, 
    reg_o => reg_o_rfadcfifo,  
    
    adc0_data => adc0_axis_tdata, 
    adc1_data => adc1_axis_tdata,   
    adc2_data => adc2_axis_tdata, 
    adc3_data => adc3_axis_tdata    
 );  
  
  

--embedded event receiver
evr: entity work.evr_top 
  generic map (
    SIM_MODE => SIM_MODE
  )
  port map(
    sys_clk => pl_clk0,
    sys_rst => pl_reset, 
    reg_o => reg_o_evr,
    refclk_p => gty_evr_refclk_p,  -- 312.5 MHz reference clock
    refclk_n => gty_evr_refclk_n,
    tx_p => gty_evr_tx_p,
    tx_n => gty_evr_tx_n,
    rx_p => gty_evr_rx_p,
    rx_n => gty_evr_rx_n,
      
    trignum => evr_dma_trignum, 
    trigdly => (x"00000001"), 
    tbt_trig => evr_tbt_trig, 
    fa_trig => evr_fa_trig, 
    sa_trig => evr_sa_trig, 
    usr_trig => evr_dma_trig, 
    gps_trig => evr_gps_trig, 
    timestamp => evr_ts,  
    evr_rcvd_clk => evr_rcvd_clk,
    evr_ref_clk => evr_ref_clk,
    dbg => evr_dbg
);	




system_i: component system
  port map (
    pl_clk0 => pl_clk0,
    pl_resetn => pl_resetn,
    -- axi-lite interface for register I/O    
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
    --adc resets
    m2_axis_aresetn_0 => pl_resetn,  
    m3_axis_aresetn_0 => pl_resetn,
    --adc clock input
    adc2_clk_clk_p => clk104_adc_refclk_p,
    adc2_clk_clk_n => clk104_adc_refclk_n,
    sysref_in_diff_p => clk104_pl_sysref_p,
    sysref_in_diff_n => clk104_pl_sysref_n,
    --adc inputs
    vin2_01_v_p => adc0_in_p,
    vin2_01_v_n => adc0_in_n, 
    vin2_23_v_p => adc1_in_p,
    vin2_23_v_n => adc1_in_n, 
    vin3_01_v_p => adc2_in_p,
    vin3_01_v_n => adc2_in_n,         
    vin3_23_v_p => adc3_in_p,
    vin3_23_v_n => adc3_in_n,       
    --clock output to drive axis clock via PLL  
    clk_adc2_0 => rfadc_out_clk, 
    clk_adc3_0 => open,
    --axis clock    
    m2_axis_aclk_0 => rfadc_axis_clk,
    m3_axis_aclk_0 => rfadc_axis_clk,
    --axis data
    m20_axis_0_tready => '1',    
    m20_axis_0_tdata => adc0_axis_tdata, 
    m20_axis_0_tvalid => adc0_axis_tvalid,
    m22_axis_0_tready => '1',    
    m22_axis_0_tdata => adc1_axis_tdata, 
    m22_axis_0_tvalid => adc1_axis_tvalid,
    m30_axis_0_tready => '1',    
    m30_axis_0_tdata => adc2_axis_tdata, 
    m30_axis_0_tvalid => adc2_axis_tvalid,    
    m32_axis_0_tready => '1',    
    m32_axis_0_tdata => adc3_axis_tdata, 
    m32_axis_0_tvalid => adc3_axis_tvalid       
    );



end behv;
