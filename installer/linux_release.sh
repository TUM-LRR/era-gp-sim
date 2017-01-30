#!/bin/bash

# This script performs all neccessary steps for a LINUX release

# Call it with  <path to qt-installation-framework-binarycreator> 
#		<cmake prefix path> 
#		<Name of the output file>

ERA_SIM_ROOT=..
ERA_SIM_EXE=era-sim
DATA_BASE=$ERA_SIM_ROOT/installer/packages/erasim.base/data/base.7z
DATA_CONFIG=$ERA_SIM_ROOT/installer/packages/erasim.config/data/config.7z
#Perform a clean build

cd $ERA_SIM_ROOT
mkdir -p build
cd build
#make clean
cmake .. -DCMAKE_PREFIX_PATH=$2
make -j4

# Successful build

# Now we zip the data for base (exe)
7z a $DATA_BASE $ERA_SIM_ROOT/build/bin/$ERA_SIM_EXE
# Then zip the data for config (isa & themes)
mkdir -p linux_release/.erasim
cp -a $ERA_SIM_ROOT/isa linux_release/.erasim/
cp -a $ERA_SIM_ROOT/themes linux_release/.erasim/

7z a $DATA_CONFIG $ERA_SIM_ROOT/build/linux_release/.erasim

# Now we run the binarycreator utility
mkdir -p $ERA_SIM_ROOT/releases
$1 -c $ERA_SIM_ROOT/installer/config/config.xml -p $ERA_SIM_ROOT/installer/packages/ $ERA_SIM_ROOT/releases/$3

# Cleanup
rm $DATA_BASE
rm $DATA_CONFIG
rm -r $ERA_SIM_ROOT/build/linux_release/.erasim

