
## SI5347 OUT6  150MHz FMC_HPC0_GBTCLK_0  (MGT_229_REFCLK_0)
set_property PACKAGE_PIN U34 [get_ports gty_evr_refclk_n]
set_property PACKAGE_PIN U33 [get_ports gty_evr_refclk_p]
create_clock -period 3.200 -name clk_gty_evr_refclk_0 [get_ports gty_evr_refclk_p]


