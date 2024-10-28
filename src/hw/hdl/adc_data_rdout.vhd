----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 02/22/2022 11:55:16 AM
-- Design Name: 
-- Module Name: adc_data_rdout - behv
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;



-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity adc_data_rdout is
  port (
    sys_clk         : in std_logic; 
    sys_rst         : in std_logic;
    adc_clk         : in std_logic; 
    adc_data        : in std_logic_vector(191 downto 0);
    fifo_trig       : in std_logic;
    fifo_rst        : in std_logic; 
    fifo_rdstr      : in std_logic; 
    fifo_dout       : out std_logic_vector(31 downto 0); 
    fifo_rdcnt      : out std_logic_vector(31 downto 0)
 );
end adc_data_rdout;

architecture behv of adc_data_rdout is



component adc_fifo IS
  PORT (
    wr_clk : IN STD_LOGIC;
    wr_rst : IN STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    rd_rst : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(255 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
    rd_data_count : OUT STD_LOGIC_VECTOR(17 DOWNTO 0)
  );
END component;


  type     state_type is (IDLE, ARM, WR_FIFO);                   
  signal   state   : state_type;  


  signal fifo_rdstr_prev  : std_logic  := '0';
  signal fifo_rdstr_fe    : std_logic  := '0';
  signal fifo_din         : std_logic_vector(255 downto 0);
  signal fifo_wren        : std_logic := '0';  

  signal adc_enb_sr       : std_logic_vector(2 downto 0);  
  signal adc_enb_s        : std_logic;
  signal sample_num       : std_logic_vector(15 downto 0);
  signal fifo_rd_data_cnt : std_logic_vector(17 downto 0);
  

  attribute mark_debug                 : string;

  attribute mark_debug of sys_rst: signal is "true";
  attribute mark_debug of adc_data: signal is "true";
  attribute mark_debug of fifo_din: signal is "true";
  attribute mark_debug of fifo_trig: signal is "true";
  attribute mark_debug of state: signal is "true";
  attribute mark_debug of sample_num: signal is "true";
  attribute mark_debug of fifo_wren: signal is "true";
  attribute mark_debug of fifo_rdstr: signal is "true";
  attribute mark_debug of fifo_rd_data_cnt: signal is "true";
  attribute mark_debug of adc_enb_sr: signal is "true";
  attribute mark_debug of adc_enb_s: signal is "true";
  attribute mark_debug of fifo_rst: signal is "true";

begin



fifo_rdcnt <= 14d"0" & fifo_rd_data_cnt;

--since fifo is fall-through mode, want the rdstr
--to happen after the current word is read.
process (sys_clk)
  begin
    if (rising_edge(sys_clk)) then
      fifo_rdstr_prev <= fifo_rdstr;
      if (fifo_rdstr = '0' and fifo_rdstr_prev = '1') then
        fifo_rdstr_fe <= '1'; --falling edge
      else
        fifo_rdstr_fe <= '0';
      end if;
    end if;
end process;
        


-- sync adc_enb to adc clock domain
process (adc_clk)
begin
  if (rising_edge(adc_clk)) then
	if (sys_rst = '1') then
	  adc_enb_sr <= "000";
    else
      adc_enb_sr(0) <= fifo_trig;
      adc_enb_sr(1) <= adc_enb_sr(0);
      adc_enb_sr(2) <= adc_enb_sr(1);
    end if;
    if (adc_enb_sr(2) = '0' and adc_enb_sr(1) = '1') then
      adc_enb_s <= '1';
    else
      adc_enb_s <= '0';
    end if;
  end if;
end process;





--write samples to fifo
process(adc_clk)
  begin
     if (rising_edge(adc_clk)) then
       if (sys_rst = '1') then
          fifo_wren <= '0';
          sample_num <= (others => '0');
          state <= idle;
          fifo_din <= (others => '0');
       else
         case state is
           when IDLE =>  
             fifo_wren <= '0'; 
             fifo_din <= (others => '0');
             sample_num <= (others => '0');    
             if (adc_enb_s = '1') then
                state <= arm;
             end if;
             
           when ARM =>  
             --if (tbt_trig = '1') then
                state <= wr_fifo;
             --end if;
             
           when WR_FIFO =>
              fifo_wren <= '1';
              fifo_din <= adc_data(15 downto 0) & adc_data(31 downto 16) & adc_data(47 downto 32) & adc_data(63 downto 48) &
                          adc_data(79 downto 64) & adc_data(95 downto 80) & adc_data(111 downto 96) & adc_data(127 downto 112) & 
                          adc_data(143 downto 128) & adc_data(159 downto 144) & adc_data(175 downto 160) & adc_data(191 downto 176) &
                          64d"0";
                          
              sample_num <= sample_num + 1;
              if (sample_num = 32d"16000") then
                state <= idle;
              else
                sample_num <= sample_num + 1;
              end if;
              
          when OTHERS => 
              state <= idle;    
              
         end case;
       end if;
     end if;
end process; 





fifo_inst : adc_fifo
  PORT MAP (
    wr_rst => fifo_rst,
    wr_clk => adc_clk,
    rd_rst => fifo_rst,
    rd_clk => sys_clk,
    din => fifo_din,
    wr_en => fifo_wren,
    rd_en => fifo_rdstr_fe,
    dout => fifo_dout,
    full => open,
    empty => open,
    rd_data_count => fifo_rd_data_cnt
  );



end behv;