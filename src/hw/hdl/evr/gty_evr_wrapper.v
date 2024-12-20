
`timescale 1ps/1ps

// =====================================================================================================================
// This example design wrapper module instantiates the core and any helper blocks which the user chose to exclude from
// the core, connects them as appropriate, and maps enabled ports
// =====================================================================================================================

module gty_evr_example_wrapper (
  input  wire [0:0] gtyrxn_in
 ,input  wire [0:0] gtyrxp_in
 ,output wire [0:0] gtytxn_out
 ,output wire [0:0] gtytxp_out
 ,input  wire [0:0] gtwiz_userclk_tx_reset_in
 ,output wire [0:0] gtwiz_userclk_tx_srcclk_out
 ,output wire [0:0] gtwiz_userclk_tx_usrclk_out
 ,output wire [0:0] gtwiz_userclk_tx_usrclk2_out
 ,output wire [0:0] gtwiz_userclk_tx_active_out
 ,input  wire [0:0] gtwiz_userclk_rx_reset_in
 ,output wire [0:0] gtwiz_userclk_rx_srcclk_out
 ,output wire [0:0] gtwiz_userclk_rx_usrclk_out
 ,output wire [0:0] gtwiz_userclk_rx_usrclk2_out
 ,output wire [0:0] gtwiz_userclk_rx_active_out
 ,input  wire [0:0] gtwiz_reset_clk_freerun_in
 ,input  wire [0:0] gtwiz_reset_all_in
 ,input  wire [0:0] gtwiz_reset_tx_pll_and_datapath_in
 ,input  wire [0:0] gtwiz_reset_tx_datapath_in
 ,input  wire [0:0] gtwiz_reset_rx_pll_and_datapath_in
 ,input  wire [0:0] gtwiz_reset_rx_datapath_in
 ,output wire [0:0] gtwiz_reset_rx_cdr_stable_out
 ,output wire [0:0] gtwiz_reset_tx_done_out
 ,output wire [0:0] gtwiz_reset_rx_done_out
 ,input  wire [15:0] gtwiz_userdata_tx_in
 ,output wire [15:0] gtwiz_userdata_rx_out
 ,input  wire [0:0] drpclk_in
 ,input  wire [0:0] gtrefclk0_in
 ,input  wire [0:0] rx8b10ben_in
 ,input  wire [0:0] rxcommadeten_in
 ,input  wire [0:0] rxmcommaalignen_in
 ,input  wire [0:0] rxpcommaalignen_in
 ,input  wire [0:0] tx8b10ben_in
 ,input  wire [15:0] txctrl0_in
 ,input  wire [15:0] txctrl1_in
 ,input  wire [7:0] txctrl2_in
 ,output wire [0:0] cpllfbclklost_out
 ,output wire [0:0] cplllock_out
 ,output wire [0:0] cpllrefclklost_out
 ,output wire [0:0] gtpowergood_out
 ,output wire [0:0] rxbyteisaligned_out
 ,output wire [0:0] rxbyterealign_out
 ,output wire [0:0] rxcommadet_out
 ,output wire [15:0] rxctrl0_out
 ,output wire [15:0] rxctrl1_out
 ,output wire [7:0] rxctrl2_out
 ,output wire [7:0] rxctrl3_out
 ,output wire [0:0] rxpmaresetdone_out
 ,output wire [0:0] txpmaresetdone_out
);


// Function to calculate a pointer to a master channel's packed index
function integer f_calc_pk_mc_idx (
  input integer idx_mc
);
begin : main_f_calc_pk_mc_idx
  integer i, j;
  integer tmp;
  j = 0;
  for (i = 0; i < 192; i = i + 1) begin
    if (P_CHANNEL_ENABLE[i] == 1'b1) begin
      if (i == idx_mc)
        tmp = j;
      else
        j = j + 1;
    end
  end
  f_calc_pk_mc_idx = tmp;
end
endfunction



  // ===================================================================================================================
  // PARAMETERS AND FUNCTIONS
  // ===================================================================================================================

  // Declare and initialize local parameters and functions used for HDL generation
  localparam [191:0] P_CHANNEL_ENABLE = 192'b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000;
  //`include "gty_evr_example_wrapper_functions.v"
  localparam integer P_TX_MASTER_CH_PACKED_IDX = f_calc_pk_mc_idx(9);
  localparam integer P_RX_MASTER_CH_PACKED_IDX = f_calc_pk_mc_idx(9);


  // ===================================================================================================================
  // HELPER BLOCKS
  // ===================================================================================================================

  // Any helper blocks which the user chose to exclude from the core will appear below. In addition, some signal
  // assignments related to optionally-enabled ports may appear below.

  // -------------------------------------------------------------------------------------------------------------------
  // Transmitter user clocking network helper block
  // -------------------------------------------------------------------------------------------------------------------

  wire [0:0] txusrclk_int;
  wire [0:0] txusrclk2_int;
  wire [0:0] txoutclk_int;

  // Generate a single module instance which is driven by a clock source associated with the master transmitter channel,
  // and which drives TXUSRCLK and TXUSRCLK2 for all channels

  // The source clock is TXOUTCLK from the master transmitter channel
  assign gtwiz_userclk_tx_srcclk_out = txoutclk_int[P_TX_MASTER_CH_PACKED_IDX];

  // Instantiate a single instance of the transmitter user clocking network helper block
  gty_evr_example_gtwiz_userclk_tx gtwiz_userclk_tx_inst (
    .gtwiz_userclk_tx_srcclk_in   (gtwiz_userclk_tx_srcclk_out),
    .gtwiz_userclk_tx_reset_in    (gtwiz_userclk_tx_reset_in),
    .gtwiz_userclk_tx_usrclk_out  (gtwiz_userclk_tx_usrclk_out),
    .gtwiz_userclk_tx_usrclk2_out (gtwiz_userclk_tx_usrclk2_out),
    .gtwiz_userclk_tx_active_out  (gtwiz_userclk_tx_active_out)
  );

  // Drive TXUSRCLK and TXUSRCLK2 for all channels with the respective helper block outputs
  assign txusrclk_int  = {1{gtwiz_userclk_tx_usrclk_out}};
  assign txusrclk2_int = {1{gtwiz_userclk_tx_usrclk2_out}};

  // -------------------------------------------------------------------------------------------------------------------
  // Receiver user clocking network helper block
  // -------------------------------------------------------------------------------------------------------------------

  wire [0:0] rxusrclk_int;
  wire [0:0] rxusrclk2_int;
  wire [0:0] rxoutclk_int;

  // Generate a single module instance which is driven by a clock source associated with the master receiver channel,
  // and which drives RXUSRCLK and RXUSRCLK2 for all channels

  // The source clock is RXOUTCLK from the master receiver channel
  assign gtwiz_userclk_rx_srcclk_out = rxoutclk_int[P_RX_MASTER_CH_PACKED_IDX];

  // Instantiate a single instance of the receiver user clocking network helper block
  gty_evr_example_gtwiz_userclk_rx gtwiz_userclk_rx_inst (
    .gtwiz_userclk_rx_srcclk_in   (gtwiz_userclk_rx_srcclk_out),
    .gtwiz_userclk_rx_reset_in    (gtwiz_userclk_rx_reset_in),
    .gtwiz_userclk_rx_usrclk_out  (gtwiz_userclk_rx_usrclk_out),
    .gtwiz_userclk_rx_usrclk2_out (gtwiz_userclk_rx_usrclk2_out),
    .gtwiz_userclk_rx_active_out  (gtwiz_userclk_rx_active_out)
  );

  // Drive RXUSRCLK and RXUSRCLK2 for all channels with the respective helper block outputs
  assign rxusrclk_int  = {1{gtwiz_userclk_rx_usrclk_out}};
  assign rxusrclk2_int = {1{gtwiz_userclk_rx_usrclk2_out}};
  wire [0:0] gtpowergood_int;

  // Required assignment to expose the GTPOWERGOOD port per user request
  assign gtpowergood_out = gtpowergood_int;
  wire [0:0] cplllock_int;

  // Required assignment to expose the CPLLLOCK port per user request
  assign cplllock_out = cplllock_int;

  // ----------------------------------------------------------------------------------------------------------------
  // Assignments to expose data ports, or data control ports, per configuration requirement or user request
  // ----------------------------------------------------------------------------------------------------------------

  wire [15:0] txctrl0_int;

  // Required assignment to expose the TXCTRL0 port per configuration requirement or user request
  assign txctrl0_int = txctrl0_in;
  wire [15:0] txctrl1_int;

  // Required assignment to expose the TXCTRL1 port per configuration requirement or user request
  assign txctrl1_int = txctrl1_in;
  wire [15:0] rxctrl0_int;

  // Required assignment to expose the RXCTRL0 port per configuration requirement or user request
  assign rxctrl0_out = rxctrl0_int;
  wire [15:0] rxctrl1_int;

  // Required assignment to expose the RXCTRL1 port per configuration requirement or user request
  assign rxctrl1_out = rxctrl1_int;


  // ===================================================================================================================
  // CORE INSTANCE
  // ===================================================================================================================

  // Instantiate the core, mapping its enabled ports to example design ports and helper blocks as appropriate
  gty_evr gty_evr_inst (
    .gtyrxn_in                               (gtyrxn_in)
   ,.gtyrxp_in                               (gtyrxp_in)
   ,.gtytxn_out                              (gtytxn_out)
   ,.gtytxp_out                              (gtytxp_out)
   ,.gtwiz_userclk_tx_reset_in               (gtwiz_userclk_tx_reset_in)
   ,.gtwiz_userclk_tx_active_in              (gtwiz_userclk_tx_active_out)
   ,.gtwiz_userclk_rx_active_in              (gtwiz_userclk_rx_active_out)
   ,.gtwiz_reset_clk_freerun_in              (gtwiz_reset_clk_freerun_in)
   ,.gtwiz_reset_all_in                      (gtwiz_reset_all_in)
   ,.gtwiz_reset_tx_pll_and_datapath_in      (gtwiz_reset_tx_pll_and_datapath_in)
   ,.gtwiz_reset_tx_datapath_in              (gtwiz_reset_tx_datapath_in)
   ,.gtwiz_reset_rx_pll_and_datapath_in      (gtwiz_reset_rx_pll_and_datapath_in)
   ,.gtwiz_reset_rx_datapath_in              (gtwiz_reset_rx_datapath_in)
   ,.gtwiz_reset_rx_cdr_stable_out           (gtwiz_reset_rx_cdr_stable_out)
   ,.gtwiz_reset_tx_done_out                 (gtwiz_reset_tx_done_out)
   ,.gtwiz_reset_rx_done_out                 (gtwiz_reset_rx_done_out)
   ,.gtwiz_userdata_tx_in                    (gtwiz_userdata_tx_in)
   ,.gtwiz_userdata_rx_out                   (gtwiz_userdata_rx_out)
   ,.drpclk_in                               (drpclk_in)
   ,.gtrefclk0_in                            (gtrefclk0_in)
   ,.rx8b10ben_in                            (rx8b10ben_in)
   ,.rxcommadeten_in                         (rxcommadeten_in)
   ,.rxmcommaalignen_in                      (rxmcommaalignen_in)
   ,.rxpcommaalignen_in                      (rxpcommaalignen_in)
   ,.rxpolarity_in                           (1'H1)
   ,.rxusrclk_in                             (rxusrclk_int)
   ,.rxusrclk2_in                            (rxusrclk2_int)
   ,.tx8b10ben_in                            (tx8b10ben_in)
   ,.txctrl0_in                              (txctrl0_int)
   ,.txctrl1_in                              (txctrl1_int)
   ,.txctrl2_in                              (txctrl2_in)
   ,.txusrclk_in                             (txusrclk_int)
   ,.txusrclk2_in                            (txusrclk2_int)
   ,.cpllfbclklost_out                       (cpllfbclklost_out)
   ,.cplllock_out                            (cplllock_int)
   ,.cpllrefclklost_out                      (cpllrefclklost_out)
   ,.gtpowergood_out                         (gtpowergood_int)
   ,.rxbyteisaligned_out                     (rxbyteisaligned_out)
   ,.rxbyterealign_out                       (rxbyterealign_out)
   ,.rxcommadet_out                          (rxcommadet_out)
   ,.rxctrl0_out                             (rxctrl0_int)
   ,.rxctrl1_out                             (rxctrl1_int)
   ,.rxctrl2_out                             (rxctrl2_out)
   ,.rxctrl3_out                             (rxctrl3_out)
   ,.rxoutclk_out                            (rxoutclk_int)
   ,.rxpmaresetdone_out                      (rxpmaresetdone_out)
   ,.txoutclk_out                            (txoutclk_int)
   ,.txpmaresetdone_out                      (txpmaresetdone_out)
);

endmodule