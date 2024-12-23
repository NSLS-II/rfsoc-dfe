##################################################################
# CHECK VIVADO VERSION
##################################################################

set scripts_vivado_version 2022.2
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
  catch {common::send_msg_id "IPS_TCL-100" "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_ip_tcl to create an updated script."}
  return 1
}

##################################################################
# START
##################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source rfadc_clk_pll.tcl
# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
  create_project rfsoc-dfe_hw rfsoc-dfe_hw -part xczu47dr-ffvg1517-1-e
  set_property target_language VHDL [current_project]
  set_property simulator_language Mixed [current_project]
}

##################################################################
# CHECK IPs
##################################################################

set bCheckIPs 1
set bCheckIPsPassed 1
if { $bCheckIPs == 1 } {
  set list_check_ips { xilinx.com:ip:clk_wiz:6.0 }
  set list_ips_missing ""
  common::send_msg_id "IPS_TCL-1001" "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

  foreach ip_vlnv $list_check_ips {
  set ip_obj [get_ipdefs -all $ip_vlnv]
  if { $ip_obj eq "" } {
    lappend list_ips_missing $ip_vlnv
    }
  }

  if { $list_ips_missing ne "" } {
    catch {common::send_msg_id "IPS_TCL-105" "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
    set bCheckIPsPassed 0
  }
}

if { $bCheckIPsPassed != 1 } {
  common::send_msg_id "IPS_TCL-102" "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 1
}

##################################################################
# CREATE IP rfadc_clk_pll
##################################################################

set rfadc_clk_pll [create_ip -name clk_wiz -vendor xilinx.com -library ip -version 6.0 -module_name rfadc_clk_pll]

# User Parameters
set_property -dict [list \
  CONFIG.CLKIN1_JITTER_PS {66.66000000000001} \
  CONFIG.CLKOUT1_JITTER {85.122} \
  CONFIG.CLKOUT1_PHASE_ERROR {85.475} \
  CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {400} \
  CONFIG.MMCM_CLKFBOUT_MULT_F {8.000} \
  CONFIG.MMCM_CLKIN1_PERIOD {6.667} \
  CONFIG.MMCM_CLKIN2_PERIOD {10.0} \
  CONFIG.MMCM_CLKOUT0_DIVIDE_F {3.000} \
  CONFIG.MMCM_DIVCLK_DIVIDE {1} \
  CONFIG.PRIM_IN_FREQ {150} \
] [get_ips rfadc_clk_pll]

# Runtime Parameters
set_property -dict { 
  GENERATE_SYNTH_CHECKPOINT {1}
} $rfadc_clk_pll

##################################################################

