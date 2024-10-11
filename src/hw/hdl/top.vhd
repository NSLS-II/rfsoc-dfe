
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
    fp_led              : out std_logic_vector(7 downto 0)

  );
end top;


architecture behv of top is

  
  signal pl_clk0      : std_logic;
  signal pl_resetn    : std_logic;
  
  signal m_axi4_m2s : t_pl_regs_m2s;
  signal m_axi4_s2m : t_pl_regs_s2m;
  
  signal reg_i      : t_addrmap_pl_regs_in;
  signal reg_o      : t_addrmap_pl_regs_out;
  


begin

fp_led <= reg_o.FP_LEDS.val.data;
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
    m_axi_wvalid => m_axi4_m2s.wvalid
    );





end behv;
