create_ip -name clk_wiz -vendor xilinx.com -library ip -version 6.0 -module_name rfadc_clk_pll
set_property -dict [list \
  CONFIG.CLKIN1_JITTER_PS {80.0} \
  CONFIG.CLKOUT1_JITTER {161.295} \
  CONFIG.CLKOUT1_PHASE_ERROR {222.305} \
  CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {200} \
  CONFIG.Component_Name {rfadc_clk_pll} \
  CONFIG.MMCM_CLKFBOUT_MULT_F {48.000} \
  CONFIG.MMCM_CLKIN1_PERIOD {8.000} \
  CONFIG.MMCM_CLKOUT0_DIVIDE_F {6.000} \
  CONFIG.MMCM_DIVCLK_DIVIDE {5} \
  CONFIG.PRIM_IN_FREQ {125} \
] [get_ips rfadc_clk_pll]
