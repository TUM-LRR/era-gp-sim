#!bin/sh

# This script performs all neccessary steps for a LINUX release

# Call it with  <path to qt-installation-framework-binarycreator> 
#		<cmake prefix path> 
#		<Name of the output file>

ERA_SIM_ROOT=../
ERA_SIM_EXE=era-sim
DATA_BASE=$ERA_SIM_ROOT/installer/packages/erasim.base/data/base.7z
DATA_CONFIG=$ERA_SIM_ROOT/installer/packages/erasim.config/data/config.7z
#Perform a clean build

cd $ERA_SIM_ROOT
mkdir build
cd build
TMP=$CMAKE_PREFIX_PATH
export CMAKE_PREFIX_PATH=$2
cmake ..
export CMAKE_PREFIX_PATH=$TMP
make -j4
if [ $ERRNO -lt 0 ]
then 
echo "Got compile errors, aborting"
return -1
fi

# Successful build

# Now we zip the data for base (exe & isa-Files)
7z a $DATA_BASE $ERA_SIM_ROOT/build/bin/$ERA_SIM_EXE $ERA_SIM_ROOT/isa
# Then zip the data for config (.erasim & themes)
7z a $DATA_CONFIG $ERA_SIM_ROOT/.erasim $ERA_SIM_ROOT/themes

# Now we run the binarycreator utility
$1 -c $ERA_SIM_ROOT/installer/config/config.xml -p $ERA_SIM_ROOT/installer/packages/ $ERA_SIM_ROOT/releases/$3

# Cleanup
rm $DATA_BASE
rm $DATA_CONFIG

