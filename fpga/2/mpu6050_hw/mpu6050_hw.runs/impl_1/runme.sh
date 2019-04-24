#!/bin/sh

# 
# Vivado(TM)
# runme.sh: a Vivado-generated Runs Script for UNIX
# Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
# 

if [ -z "$PATH" ]; then
  PATH=/media/kiott01a/SAMSUNG/Xilinx/SDK/2018.2/bin:/media/kiott01a/SAMSUNG/Xilinx/Vivado/2018.2/ids_lite/ISE/bin/lin64:/media/kiott01a/SAMSUNG/Xilinx/Vivado/2018.2/bin
else
  PATH=/media/kiott01a/SAMSUNG/Xilinx/SDK/2018.2/bin:/media/kiott01a/SAMSUNG/Xilinx/Vivado/2018.2/ids_lite/ISE/bin/lin64:/media/kiott01a/SAMSUNG/Xilinx/Vivado/2018.2/bin:$PATH
fi
export PATH

if [ -z "$LD_LIBRARY_PATH" ]; then
  LD_LIBRARY_PATH=/media/kiott01a/SAMSUNG/Xilinx/Vivado/2018.2/ids_lite/ISE/lib/lin64
else
  LD_LIBRARY_PATH=/media/kiott01a/SAMSUNG/Xilinx/Vivado/2018.2/ids_lite/ISE/lib/lin64:$LD_LIBRARY_PATH
fi
export LD_LIBRARY_PATH

HD_PWD='/home/kiott01a/kiotta/fpga/2/mpu6050_hw/mpu6050_hw.runs/impl_1'
cd "$HD_PWD"

HD_LOG=runme.log
/bin/touch $HD_LOG

ISEStep="./ISEWrap.sh"
EAStep()
{
     $ISEStep $HD_LOG "$@" >> $HD_LOG 2>&1
     if [ $? -ne 0 ]
     then
         exit
     fi
}

# pre-commands:
/bin/touch .write_bitstream.begin.rst
EAStep vivado -log i2c_mpu6050_wrapper.vdi -applog -m64 -product Vivado -messageDb vivado.pb -mode batch -source i2c_mpu6050_wrapper.tcl -notrace

