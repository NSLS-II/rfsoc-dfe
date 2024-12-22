


set_clock_groups -name clkpl0_2_rfadcclk -asynchronous -group [get_clocks clk_pl_0] -group [get_clocks -of_objects [get_pins axisclk_adc/inst/mmcme4_adv_inst/CLKOUT0]]


