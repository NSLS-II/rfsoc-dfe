
--//////////////////////////////////////////////////////////////////////////////////
--// Company: 
--// Engineer: 
--// 
--// Create Date: 05/14/2015 02:56:06 PM
--// Design Name: 
--// Module Name: evr_top
--// Project Name: 
--// Target Devices: 
--// Tool Versions: 
--// Description: 
--// 
--// Dependencies: 
--// 
--// Revision:
--// Revision 0.01 - File Created
--// Additional Comments:
--//

--//	SFP 0    - X0Y2   --- EVR Port
--//
--// 
--//////////////////////////////////////////////////////////////////////////////////

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;


library xil_defaultlib;
use xil_defaultlib.bpm_package.ALL;


entity evr_top is
  generic (
    SIM_MODE      : integer := 0
  );
  port(

    sys_clk        : in std_logic;
    sys_rst        : in std_logic;
    reg_o          : in t_reg_o_evr;
    --evr_gty_reset  : in std_logic_vector(7 downto 0);
    
    refclk_p       : in std_logic;	 -- 312.5 MHz reference clock
    refclk_n       : in std_logic;
    
    tx_p           : out std_logic;
    tx_n           : out std_logic;
    rx_p           : in std_logic;
    rx_n           : in std_logic;

    trignum        : in std_logic_vector(7 downto 0);
    trigdly        : in std_logic_vector(31 downto 0);
    
    tbt_trig       : out std_logic;
    fa_trig        : out std_logic;
    sa_trig        : out std_logic;
    usr_trig       : out std_logic;
    gps_trig       : out std_logic;
    timestamp      : out std_logic_vector(63 downto 0);
    
    evr_rcvd_clk   : out std_logic;
    evr_ref_clk    : out std_logic;
    dbg            : out std_logic_vector(19 downto 0)
    
);
end evr_top;
 
  
architecture behv of evr_top is
	

component timeofDayReceiver is
  port (
    clock        : in std_logic;
    reset        : in std_logic; 
    eventstream  : in std_logic_vector(7 downto 0);
    timestamp    : out std_logic_vector(63 downto 0); 
    seconds      : out std_logic_vector(31 downto 0); 
    offset       : out std_logic_vector(31 downto 0); 
    position     : out std_logic_vector(4 downto 0);
    eventclock   : out std_logic
);
end component;


component EventReceiverChannel is 
  port (
    clock        : in std_logic;
    reset        : in std_logic;
    eventstream  : in std_logic_vector(7 downto 0); 
    myevent      : in std_logic_vector(7 downto 0);
    mydelay      : in std_logic_vector(31 downto 0); 
    mywidth      : in std_logic_vector(31 downto 0); 
    mypolarity   : in std_logic;
    trigger      : out std_logic 
);
end component;


component gty_evr_example_wrapper 
  port (
    gtyrxn_in                           : in std_logic;
    gtyrxp_in                           : in std_logic; 
    gtytxn_out                          : out std_logic;
    gtytxp_out                          : out std_logic; 
    gtwiz_reset_clk_freerun_in          : in std_logic; 
    gtwiz_reset_all_in                  : in std_logic;    
    drpclk_in                           : in std_logic; 
    gtrefclk0_in                        : in std_logic;   
    gtpowergood_out                     : out std_logic; 
    cpllfbclklost_out                   : out std_logic;
    cplllock_out                        : out std_logic;
    cpllrefclklost_out                  : out std_logic; 
      
    gtwiz_userdata_tx_in                : in std_logic_vector(15 downto 0);     
    txctrl0_in                          : in std_logic_vector(15 downto 0); 
    txctrl1_in                          : in std_logic_vector(15 downto 0); 
    txctrl2_in                          : in std_logic_vector(7 downto 0);  
    tx8b10ben_in                        : in std_logic; 
    gtwiz_userclk_tx_reset_in           : in std_logic;   
    gtwiz_userclk_tx_srcclk_out         : out std_logic;  
    gtwiz_userclk_tx_usrclk_out         : out std_logic; 
    gtwiz_userclk_tx_usrclk2_out        : out std_logic; 
    gtwiz_userclk_tx_active_out         : out std_logic; 
    gtwiz_reset_tx_pll_and_datapath_in  : in std_logic; 
    gtwiz_reset_tx_datapath_in          : in std_logic; 
    gtwiz_reset_tx_done_out             : out std_logic; 
    txpmaresetdone_out                  : out std_logic; 
   
    gtwiz_userdata_rx_out               : out std_logic_vector(15 downto 0); 
    rxctrl0_out                         : out std_logic_vector(15 downto 0); 
    rxctrl1_out                         : out std_logic_vector(15 downto 0); 
    rxctrl2_out                         : out std_logic_vector(7 downto 0); 
    rxctrl3_out                         : out std_logic_vector(7 downto 0); 
    rx8b10ben_in                        : in std_logic;
    rxcommadeten_in                     : in std_logic;
    rxmcommaalignen_in                  : in std_logic;
    rxpcommaalignen_in                  : in std_logic;
    gtwiz_userclk_rx_reset_in           : in std_logic; 
    gtwiz_userclk_rx_srcclk_out         : out std_logic; 
    gtwiz_userclk_rx_usrclk_out         : out std_logic;  
    gtwiz_userclk_rx_usrclk2_out        : out std_logic;  
    gtwiz_userclk_rx_active_out         : out std_logic; 
    gtwiz_reset_rx_pll_and_datapath_in  : in std_logic; 
    gtwiz_reset_rx_datapath_in          : in std_logic; 
    gtwiz_reset_rx_cdr_stable_out       : out std_logic; 
    gtwiz_reset_rx_done_out             : out std_logic; 
    rxpmaresetdone_out                  : out std_logic;
    rxbyteisaligned_out                 : out std_logic;
    rxbyterealign_out                   : out std_logic;
    rxcommadet_out                      : out std_logic
);  
end component;




  signal tbt_trig_i             : std_logic;
  signal datastream             : std_logic_vector(7 downto 0);
  signal eventstream            : std_logic_vector(7 downto 0);   
  signal eventclock             : std_logic;
   
  signal prev_datastream        : std_logic_vector(3 downto 0);
  signal cnt                    : integer range 3 downto 0;
  signal trigactive             : std_logic;

  signal evr_gty_refclk         : std_logic;

  signal evr_gty_refclk_bufg_gt : std_logic;
  signal evr_gty_refclk_odiv2   : std_logic;
   
  signal gty_txcnt              : std_logic_vector(15 downto 0);
   
  signal gty_powergood          :  std_logic;
  signal gty_cpllfbclklost      : std_logic;
  signal gty_cplllock           : std_logic;
  signal gty_cpllrefclklost     : std_logic;  
   
  signal gty_userclk_tx_srcclk : std_logic;
  signal gty_userclk_tx_usrclk : std_logic;
  signal gty_userclk_tx_usrclk2 : std_logic;
  signal gty_userclk_tx_active : std_logic; 
  signal gty_reset_tx_done     : std_logic;
  signal gty_txpmaresetdone    : std_logic;
  signal gty_txdata_in         : std_logic_vector(15 downto 0);
  signal gty_txcharisk_in      : std_logic_vector(7 downto 0);

  signal gty_userclk_rx_srcclk : std_logic;
  signal gty_userclk_rx_usrclk : std_logic;
  signal gty_userclk_rx_usrclk2 : std_logic;
  signal gty_userclk_rx_active : std_logic;
  signal gty_reset_rx_done     : std_logic;
  signal gty_rxpmaresetdone    : std_logic;
  signal gty_reset_rx_cdr_stable : std_logic;  
  signal gty_rxbyteisaligned   : std_logic;
  signal gty_rxbyterealign     : std_logic;
  signal gty_rxcommadet        : std_logic; 
  signal gty_rx_userdata       : std_logic_vector(15 downto 0);
  signal gty_rxctrl0           : std_logic_vector(15 downto 0);
  signal gty_rxctrl1           : std_logic_vector(15 downto 0);
  signal gty_rxctrl2           : std_logic_vector(7 downto 0);
  signal gty_rxctrl3           : std_logic_vector(7 downto 0);   
   
   




   --debug signals (connect to ila)
   attribute mark_debug     : string;
   attribute mark_debug of eventstream: signal is "true";
   attribute mark_debug of datastream: signal is "true";
   attribute mark_debug of timestamp: signal is "true";
   attribute mark_debug of eventclock: signal is "true";
   attribute mark_debug of prev_datastream: signal is "true";
   attribute mark_debug of tbt_trig: signal is "true";
   attribute mark_debug of tbt_trig_i: signal is "true";

   attribute mark_debug of gty_txdata_in: signal is "true";  
   attribute mark_debug of gty_txcharisk_in: signal is "true";
   attribute mark_debug of gty_rx_userdata: signal is "true";
   attribute mark_debug of gty_rxctrl0: signal is "true";
   attribute mark_debug of gty_powergood: signal is "true";
   attribute mark_debug of gty_reset_rx_done: signal is "true";

   attribute mark_debug of gty_reset_tx_done: signal is "true";
   attribute mark_debug of gty_userclk_tx_active: signal is "true";
   attribute mark_debug of gty_txpmaresetdone: signal is "true";
  
   attribute mark_debug of gty_cpllfbclklost: signal is "true";
   attribute mark_debug of gty_cplllock: signal is "true";
   attribute mark_debug of gty_cpllrefclklost: signal is "true"; 



begin

evr_rcvd_clk <= gty_userclk_rx_usrclk2;



IBUFDS_GTE4_inst : IBUFDS_GTE4
  generic map (
    REFCLK_EN_TX_PATH => '0',   -- Refer to Transceiver User Guide
    REFCLK_HROW_CK_SEL => "00", -- Refer to Transceiver User Guide
    REFCLK_ICNTL_RX => "00"     -- Refer to Transceiver User Guide
 )
  port map (
      O => evr_gty_refclk,         -- 1-bit output: Refer to Transceiver User Guide
      ODIV2 => evr_gty_refclk_odiv2,  -- 1-bit output: Refer to Transceiver User Guide
      CEB => '0',     -- 1-bit input: Refer to Transceiver User Guide
      I => refclk_p,         -- 1-bit input: Refer to Transceiver User Guide
      IB => refclk_n        -- 1-bit input: Refer to Transceiver User Guide
   );


BUFG_GT_inst : BUFG_GT
   port map (
      O => evr_ref_clk,             -- 1-bit output: Buffer
      CE => '1',            -- 1-bit input: Buffer enable
      CEMASK => '1',    -- 1-bit input: CE Mask
      CLR => '0',          -- 1-bit input: Asynchronous clear
      CLRMASK => '1', -- 1-bit input: CLR Mask
      DIV => "000",         -- 3-bit input: Dynamic divide Value
      I => evr_gty_refclk_odiv2              -- 1-bit input: Buffer
   );


-- send test data out tx port for loopback tests
process(gty_userclk_tx_usrclk2)
  begin
    if (rising_edge(gty_userclk_tx_usrclk2)) then
      if (sys_rst = '1') then --(gth_reset(0) = '1') then
         gty_txdata_in <= x"50BC";
         gty_txcharisk_in <= x"01";
         gty_txcnt <= 16d"0";
      else
         if (gty_txcnt = 16d"100") then
           gty_txdata_in <= x"50BC";
           gty_txcharisk_in <= x"01";            
           gty_txcnt <= 16d"0";
         else
           gty_txdata_in <= gty_txcnt;
           gty_txcharisk_in <= x"00";
           gty_txcnt <= gty_txcnt + 1;
         end if;
      end if;
    end if;
end process;






--stretch tbt_trig for a few clock cycles
process (evr_rcvd_clk)
begin 
   if (rising_edge(evr_rcvd_clk)) then
      if (sys_rst = '1') then
         tbt_trig <= '0';
         cnt <= 0;
         trigactive <= '0';
      else        
         if (tbt_trig_i = '1') then
            tbt_trig <= '1';
            trigactive <= '1';
         end if;
         if (trigactive = '1') then
           if (cnt = 3) then
             tbt_trig <= '0';
             trigactive <= '0';
             cnt <= 0;
           else
             cnt <= cnt + 1;
           end if;
         end if;
      end if;
   end if;
end process;

 


evr_sim: if (SIM_MODE = 1) generate evr_tb:

-- generate evr clock - need to adjust to correct frequency, use internal tbt-trig for simulation
 process
     begin
         evr_rcvd_clk <= '0';
         wait for 4 ns;
         evr_rcvd_clk <= '1';
         wait for 4 ns;
end process;

--tbt gen 
process 
  begin
    tbt_trig_i <= '0';
    wait for 4 ns;
    tbt_trig_i <= '1';
    wait for 8 ns;
    tbt_trig_i <= '0';
    wait for 8*310 ns;

end process;




end generate;



evr_syn: if (SIM_MODE = 0) generate evr_logic:

--tbt_trig <= datastream(0);
--datastream 0 toggles high/low for half of Frev.  Filter on the first low to high transition
--and ignore the rest
process (sys_rst, evr_rcvd_clk)
begin
    if (sys_rst = '1') then
       tbt_trig_i <= '0';
    elsif (evr_rcvd_clk'event and evr_rcvd_clk = '1') then
       prev_datastream(0) <= datastream(0);
       prev_datastream(1) <= prev_datastream(0);
       prev_datastream(2) <= prev_datastream(1);
       prev_datastream(3) <= prev_datastream(2);
       if (prev_datastream = "0001") then
           tbt_trig_i <= '1';
       else
           tbt_trig_i <= '0';
       end if;
    end if;
end process;


--datastream <= gt0_rxdata(7 downto 0);
--eventstream <= gt0_rxdata(15 downto 8);
--switch byte locations of datastream and eventstream  9-20-18
datastream <= gty_rx_userdata(15 downto 8);
eventstream <= gty_rx_userdata(7 downto 0);



	
-- timestamp decoder
ts : timeofDayReceiver
   port map(
       clock => evr_rcvd_clk,
       reset => sys_rst,
       eventstream => eventstream,
       timestamp => timestamp,
       seconds => open, 
       offset => open, 
       position => open, 
       eventclock => eventclock
 );


-- 1 Hz GPS tick	
event_gps : EventReceiverChannel
    port map(
       clock => evr_rcvd_clk,
       reset => sys_rst,
       eventstream => eventstream,
       myevent => (x"7D"),     -- 125d
       mydelay => (x"00000001"),
       mywidth => (x"00000175"),   -- //creates a pulse about 3us long
       mypolarity => ('0'),
       trigger => gps_trig
);


-- 10 Hz 	
event_10Hz : EventReceiverChannel
    port map(
       clock => evr_rcvd_clk,
       reset => sys_rst,
       eventstream => eventstream,
       myevent => (x"1E"),     -- 30d
       mydelay => (x"00000001"),
       mywidth => (x"00000175"),   -- //creates a pulse about 3us long
       mypolarity => ('0'),
       trigger => sa_trig
);


-- 10 KHz 	
event_10KHz : EventReceiverChannel
    port map(
       clock => evr_rcvd_clk,
       reset => sys_rst,
       eventstream => eventstream,
       myevent => (x"1F"),     -- 31d
       mydelay => (x"00000001"),
       mywidth => (x"00000175"),   -- //creates a pulse about 3us long
       mypolarity => ('0'),
       trigger => fa_trig
);
		
		
-- On demand 	
event_usr : EventReceiverChannel
    port map(
       clock => evr_rcvd_clk,
       reset => sys_rst,
       eventstream => eventstream,
       myevent => trignum,
       mydelay => trigdly, 
       mywidth => (x"00000175"),   -- //creates a pulse about 3us long
       mypolarity => ('0'),
       trigger => usr_trig
);



gty : gty_evr_example_wrapper 
  port map (
    gtyrxn_in => rx_n,
    gtyrxp_in => rx_p,
    gtytxn_out => tx_n,
    gtytxp_out => tx_p,
    gtwiz_reset_clk_freerun_in => sys_clk, 
    gtwiz_reset_all_in => reg_o.reset, --evr_gty_reset(0),   
    drpclk_in => sys_clk, 
    gtrefclk0_in => evr_gty_refclk,  
    gtpowergood_out => gty_powergood,
    cpllfbclklost_out => gty_cpllfbclklost,
    cplllock_out => gty_cplllock,
    cpllrefclklost_out => gty_cpllrefclklost,    
    
      
    gtwiz_userdata_tx_in => gty_txdata_in,     
    txctrl0_in => x"0000",
    txctrl1_in => x"0000",
    txctrl2_in => gty_txcharisk_in,  
    tx8b10ben_in => '1',
    rxcommadeten_in => '1', 
    rxmcommaalignen_in => '1', 
    rxpcommaalignen_in => '1',     
    gtwiz_userclk_tx_reset_in => reg_o.reset, --evr_gty_reset(0),  
    gtwiz_userclk_tx_srcclk_out => gty_userclk_tx_srcclk, 
    gtwiz_userclk_tx_usrclk_out => gty_userclk_tx_usrclk,
    gtwiz_userclk_tx_usrclk2_out => gty_userclk_tx_usrclk2,
    gtwiz_userclk_tx_active_out => gty_userclk_tx_active,
    gtwiz_reset_tx_pll_and_datapath_in => reg_o.reset, --evr_gty_reset(0),
    gtwiz_reset_tx_datapath_in => reg_o.reset, --evr_gty_reset(0),
    gtwiz_reset_tx_done_out => gty_reset_tx_done,
    txpmaresetdone_out => gty_txpmaresetdone,
   
    gtwiz_userdata_rx_out => gty_rx_userdata,
    rxctrl0_out => gty_rxctrl0,
    rxctrl1_out => gty_rxctrl1,
    rxctrl2_out => gty_rxctrl2,
    rxctrl3_out => gty_rxctrl3,
    rx8b10ben_in => '1',
    gtwiz_userclk_rx_reset_in => reg_o.reset, --evr_gty_reset(0),
    gtwiz_userclk_rx_srcclk_out => gty_userclk_rx_srcclk,
    gtwiz_userclk_rx_usrclk_out => gty_userclk_rx_usrclk, 
    gtwiz_userclk_rx_usrclk2_out => gty_userclk_rx_usrclk2, 
    gtwiz_userclk_rx_active_out => gty_userclk_rx_active,
    gtwiz_reset_rx_pll_and_datapath_in => reg_o.reset, --evr_gty_reset(0),
    gtwiz_reset_rx_datapath_in => reg_o.reset, --evr_gty_reset(0),
    gtwiz_reset_rx_cdr_stable_out=> gty_reset_rx_cdr_stable,
    gtwiz_reset_rx_done_out => gty_reset_rx_done,
    rxpmaresetdone_out => gty_rxpmaresetdone,
    rxbyteisaligned_out => gty_rxbyteisaligned,
    rxbyterealign_out => gty_rxbyterealign, 
    rxcommadet_out => gty_rxcommadet
);	



end generate;

		 
		

			 
end behv;
