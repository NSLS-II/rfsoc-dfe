

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

library xil_defaultlib;
use xil_defaultlib.bpm_package.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity rf_adc_fifos is
  port (
    pl_clk0         : in std_logic; 
    pl_reset        : in std_logic;
    adc_clk         : in std_logic; 
    reg_i           : out t_reg_i_rfadc_fifo_rdout;
    reg_o           : in  t_reg_o_rfadc_fifo_rdout; 
    
    adc0_data       : in std_logic_vector(191 downto 0);
    adc1_data       : in std_logic_vector(191 downto 0);  
    adc2_data       : in std_logic_vector(191 downto 0);
    adc3_data       : in std_logic_vector(191 downto 0)    

 );
end rf_adc_fifos;

architecture behv of rf_adc_fifos is



begin



adc0_fifo:  entity work.adc_data_rdout
  port map (
    sys_clk => pl_clk0, 
    adc_clk => adc_clk,  
    sys_rst => pl_reset,
    adc_data => adc0_data,
    fifo_trig => reg_o.enb,  
    fifo_rdstr => reg_o.adc0_rdstr, 
    fifo_dout => reg_i.adc0_dout,  
    fifo_rdcnt => reg_i.adc0_rdcnt, 
    fifo_rst => reg_o.rst
 );

adc1_fifo:  entity work.adc_data_rdout
  port map (
    sys_clk => pl_clk0, 
    adc_clk => adc_clk,  
    sys_rst => pl_reset,
    adc_data => adc1_data,
    fifo_trig => reg_o.enb,  
    fifo_rdstr => reg_o.adc1_rdstr, 
    fifo_dout => reg_i.adc1_dout,  
    fifo_rdcnt => reg_i.adc1_rdcnt, 
    fifo_rst => reg_o.rst
 );
 
 adc2_fifo:  entity work.adc_data_rdout
  port map (
    sys_clk => pl_clk0, 
    adc_clk => adc_clk,  
    sys_rst => pl_reset,
    adc_data => adc2_data,
    fifo_trig => reg_o.enb,  
    fifo_rdstr => reg_o.adc2_rdstr, 
    fifo_dout => reg_i.adc2_dout,  
    fifo_rdcnt => reg_i.adc2_rdcnt, 
    fifo_rst => reg_o.rst
 );
 
adc3_fifo:  entity work.adc_data_rdout
  port map (
    sys_clk => pl_clk0, 
    adc_clk => adc_clk,  
    sys_rst => pl_reset,
    adc_data => adc3_data,
    fifo_trig => reg_o.enb,  
    fifo_rdstr => reg_o.adc3_rdstr, 
    fifo_dout => reg_i.adc3_dout,  
    fifo_rdcnt => reg_i.adc3_rdcnt, 
    fifo_rst => reg_o.rst
 );



end behv;