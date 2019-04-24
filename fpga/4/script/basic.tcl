set proj_name basic
set part_name xc7z010clg400-1
set ip_dir repo
set bd_name system
set bd_path $proj_name/$proj_name.srcs/sources_1/bd/$bd_name

file delete -force $proj_name

create_project -part $part_name $proj_name $proj_name

set_property board_part digilentinc.com:zybo-z7-10:part0:1.0 [current_project]

update_ip_catalog

create_bd_design $bd_name

update_compile_order -fileset sources_1

create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0

apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" } [get_bd_cells processing_system7_0]

create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0

set_property CONFIG.USE_BOARD_FLOW true [get_bd_cells /axi_gpio_0]
set_property CONFIG.GPIO_BOARD_INTERFACE Custom [get_bd_cells /axi_gpio_0]
create_bd_intf_port -mode MASTER -vlnv xilinx.com:interface:gpio_rtl:1.0 gpio_rtl
connect_bd_intf_net /gpio_rtl /axi_gpio_0/GPIO

apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_masker {Auto} Clik_Slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/axi_gpio_0/S_AXI} intc_ip {New AXI Interconnect} master_apm {0}} [get_bd_intf_pins axi_gpio_0/S_AXI]

set_property -dict [list CONFIG.C_GPIO_WIDTH {1}] [get_bd_cells axi_gpio_0]

regenerate_bd_layout
validate_bd_design
save_bd_design

generate_target all [get_files $bd_path/$bd_name.bd]
make_wrapper -files [get_files $bd_path/$bd_name.bd] -top
add_files -norecurse $bd_path/hdl/${bd_name}_wrapper.v

set_property SEVERITY {Warning} [get_drc_checks NSTD-1]
set_property SEVERITY {Warning} [get_drc_checks UCIO-1]

add_files -norecurse -fileset constrs_1 ./script/zybo_z7.xdc
import_files -fileset constrs_1 ./script/zybo_z7.xdc

set_property verilog_define {TOOL_VIVADO} [current_fileset]

set_property SEVERITY {Warning} [get_drc_checks NSTD-1]
set_property SEVERITY {Warning} [get_drc_checks UCIO-1]

launch_runs synth_1 -jobs 2
set_property SEVERITY {Warning} [get_drc_checks NSTD-1]
set_property SEVERITY {Warning} [get_drc_checks UCIO-1]
wait_on_run synth_1

launch_runs impl_1 -to_step write_bitstream -jobs 2
set_property SEVERITY {Warning} [get_drc_checks NSTD-1]
set_property SEVERITY {Warning} [get_drc_checks UCIO-1]
wait_on_run impl_1

puts "Finish to write bitstream!"

file mkdir $proj_name/$proj_name.sdk
file copy -force $proj_name/$proj_name.runs/impl_1/system_wrapper.sysdef $proj_name/$proj_name.sdk/system_wrapper.hdf

launch_sdk -workspace $proj_name/$proj_name.sdk -hwspec $proj_name/$proj_name.sdk/system_wrapper.hdf
save_bd_design
close_project
