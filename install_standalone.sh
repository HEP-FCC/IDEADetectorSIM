#!/bin/bash

# set STANDALONE_INSTALL_DIR="your_complete_installation_path", e.g.:
#export STANDALONE_INSTALL_DIR="your_installation_path"
export STANDALONE_INSTALL_DIR=/its/home/iv41/workarea/FullIDEASim_superdev
# -----------------------------------------------
echo "installing in $STANDALONE_INSTALL_DIR"
mkdir $STANDALONE_INSTALL_DIR 
cd $STANDALONE_INSTALL_DIR

# -------------------
# SIMULATION
echo "compile simulation"
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/simulation/
# set the right PRJBASE in simulation/env.sh
string1="export PRJBASE=path_to_prjbase"
string2="export PRJBASE=\"$STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex\""
sed "s|$string1|$string2|g" ./env.sh_init > env.sh
### ### ### ### ### ### ### ### ###
### only for emergency: this is to run with a specific release of key4hep stack
# string1="source /cvmfs/sw.hsf.org/key4hep/setup.sh"
# string2="source $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/key4hep_setup.sh"
# sed -i "s|$string1|$string2|g" ./env.sh
### ### ### ### ### ### ### ### ##
source ./env.sh
mkdir build $SIM_INSTAL_DIR $SIM_OUTPUT_DIR
# compile 
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/simulation/build/
cmake -DCMAKE_INSTALL_PREFIX=$SIM_INSTAL_DIR  -DCMAKE_CXX_FLAGS="-DMT_OFF"  ../g4GMC
make
make install
cd $STANDALONE_INSTALL_DIR

# -------------------
# INSTALL EXTERNAL CODE LOCALLY
mkdir -p LOCAL LOCAL/ROME

echo ""
echo ""
echo ""
# -------------------
# GENFIT
echo "setup genfit"
for path in ${CMAKE_PREFIX_PATH//:/ }; do
    if  grep -q 'genfit' <<< "$path"  ; then
	export GENFIT=$path
    fi
done
echo "GENFIT set to " $GENFIT
cd $STANDALONE_INSTALL_DIR

echo ""
echo ""
echo ""
# -------------------
# ROME
cd $STANDALONE_INSTALL_DIR/LOCAL/ROME
echo "download the ROME release"
git clone https://bitbucket.org/muegamma/rome3.git rome
cd $STANDALONE_INSTALL_DIR/LOCAL/ROME/rome
git checkout master
#git tag
#git checkout v3.2.15.1
export ROMESYS=$STANDALONE_INSTALL_DIR/LOCAL/ROME/rome
export PATH=$ROMESYS/bin:$PATH
cd $ROMESYS
make
ln -s $ROMESYS/src/strlcpy.cxx $ROMESYS/src/strlcpy.c
cd $STANDALONE_INSTALL_DIR

echo ""
echo ""
echo ""
# -------------------
# ANALYZER
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/analyzer/
# set the right PRJBASE in analyzer/envGMC.sh
string1="export PRJBASE=path_to_prjbase"
string2="export PRJBASE=\"$STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex\""
sed "s|$string1|$string2|g" ./envGMC.sh_init > envGMC.sh
# set the right GENFIT2SYS in analyzer/envGMC.sh
string1="export GENFIT2SYS=path_to_genfit2"
string2="export GENFIT2SYS=$GENFIT"
sed -i "s|$string1|$string2|g" ./envGMC.sh
# set the right ROOTSYS in analyzer/envGMC.sh
string1="export ROOTSYS=path_to_root"
string2="export ROOTSYS=$ROOTSYS"
sed -i "s|$string1|$string2|g" ./envGMC.sh
# set the right ROMESYS in analyzer/envGMC.sh
string1="export ROMESYS=path_to_rome"
string2="export ROMESYS=$ROMESYS"
sed -i "s|$string1|$string2|g" ./envGMC.sh
#
source ./envGMC.sh
echo "compile analyzer"
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/analyzer/GMC
$ROMESYS/bin/romebuilder.exe -i GMC.xml
cd $STANDALONE_INSTALL_DIR

# -------------------
# CONVERTER
echo "compile converter"
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/converter
cmake .
make VERBOSE=1
cd $STANDALONE_INSTALL_DIR

# -------------------
# GEOMETRY
echo "copy geometry GDML/XML files"
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/analyzer/GMC
cp /afs/cern.ch/work/l/llavezzi/public/geometry/* .
# set the right PATH in the xml files
string1="<SPValue>path_to_simulation</SPValue>"
string2="<SPValue>$STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/simulation/g4GMC/config</SPValue>"
sed "s|$string1|$string2|g" ./geant4MC-IDEA.xml_init > geant4MC-IDEA.xml
sed "s|$string1|$string2|g" ./geant4MC-IDEA-fit.xml_init > geant4MC-IDEA-fit.xml


cd $STANDALONE_INSTALL_DIR
mkdir -p build/AnalysisTools
cd build/AnalysisTools
cmake -DCMAKE_INSTALL_PREFIX=$STANDALONE_INSTALL_DIR/build $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/DRCalo/Analysis/AnalysisTools
make 
make install

##### checkout SiPM digitization package 


cd $STANDALONE_INSTALL_DIR
mkdir -p build/DRCalo_DRDigitization
cd build/DRCalo_DRDigitization
cmake -DCMAKE_INSTALL_PREFIX=$STANDALONE_INSTALL_DIR/build $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/DRCalo/Digitization/DRDigitization
make 
make install

export PYTHONPATH=${PYTHONPATH}:$STANDALONE_INSTALL_DIR/build/python
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$STANDALONE_INSTALL_DIR/build/lib:$STANDALONE_INSTALL_DIR/build/lib64
export PATH=${PATH}:$STANDALONE_INSTALL_DIR/build/bin



cd $STANDALONE_INSTALL_DIR
echo "finished installation in $STANDALONE_INSTALL_DIR"
