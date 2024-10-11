create_ip -name fifo_generator -vendor xilinx.com -library ip -version 13.2 -module_name adc_fifo
set_property -dict [list \
  CONFIG.Component_Name {adc_fifo} \
  CONFIG.Fifo_Implementation {Independent_Clocks_Block_RAM} \
  CONFIG.Input_Data_Width {16} \
  CONFIG.Input_Depth {2048} \
  CONFIG.Performance_Options {First_Word_Fall_Through} \
  CONFIG.Reset_Pin {false} \
  CONFIG.Use_Extra_Logic {true} \
  CONFIG.asymmetric_port_width {false} \
] [get_ips adc_fifo]
