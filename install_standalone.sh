#!/bin/bash

export STANDALONE_INSTALL_DIR="/afs/cern.ch/work/l/llavezzi/IDEA2/test"

# -----------------------------------------------
echo "installing in $STANDALONE_INSTALL_DIR"
mkdir $STANDALONE_INSTALL_DIR 
cd $STANDALONE_INSTALL_DIR

echo "download the simulation and analysis codes"
git clone https://github.com/lialavezzi/DriftChamberPLUSVertex.git
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/
git checkout uptodate

# -------------------
# SIMULATION
echo "compile simulation"
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/simulation/
# set the right PRJBASE in simulation/env.sh
string1="export PRJBASE=\"/afs/cern.ch/work/l/llavezzi/IDEA/DRIFT_CH/gitIDEA\""
string2="export PRJBASE=\"$STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex\""
sed -i "s|$string1|$string2|g" ./env.sh
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
mkdir -p LOCAL LOCAL/GENFIT LOCAL/ROME

# -------------------
# GENFIT
cd $STANDALONE_INSTALL_DIR/LOCAL/GENFIT
echo "download the genfit release"
git clone https://github.com/GenFit/GenFit.git
cd $STANDALONE_INSTALL_DIR/LOCAL/GENFIT/GenFit
git checkout release-01-00-00
export GENFIT=$STANDALONE_INSTALL_DIR/LOCAL/GENFIT
cd $GENFIT/GenFit
./makeEnv.sh
source env.sh
cd ..
mkdir build
cd build
cmake ../GenFit
make
cd $STANDALONE_INSTALL_DIR

# -------------------
# ROME
cd $STANDALONE_INSTALL_DIR/LOCAL/ROME
echo "download the ROME release"
git clone https://bitbucket.org/muegamma/rome3.git rome
cd $STANDALONE_INSTALL_DIR/LOCAL/ROME/rome
#git checkout master
#git tag
git checkout v3.2.15.1
export ROMESYS=$STANDALONE_INSTALL_DIR/LOCAL/ROME/rome
export PATH=$ROMESYS/bin:$PATH
cd $ROMESYS
make
ln -s $ROMESYS/src/strlcpy.cxx $ROMESYS/src/strlcpy.c
cd $STANDALONE_INSTALL_DIR

# -------------------
# ANALYZER
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/analyzer/
# set the right PRJBASE in analyzer/envGMC.sh
string1="export PRJBASE=\"/afs/cern.ch/work/l/llavezzi/IDEA/DRIFT_CH/gitIDEA\""
string2="export PRJBASE=\"$STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex\""
sed -i "s|$string1|$string2|g" ./envGMC.sh
# set the right GENFIT2SYS in analyzer/envGMC.sh
string1="export GENFIT2SYS=/afs/cern.ch/work/l/llavezzi/IDEA/LOCAL/GENFIT/master20191106/build"
string2="export GENFIT2SYS=$GENFIT/build"
sed -i "s|$string1|$string2|g" ./envGMC.sh
# set the right ROMESYS in analyzer/envGMC.sh
string1="export ROMESYS=/afs/cern.ch/work/l/llavezzi/IDEA/LOCAL/ROME/rome-v3.2.15.1"
string2="export ROMESYS=$ROMESYS"
sed -i "s|$string1|$string2|g" ./envGMC.sh
#
source ./envGMC.sh
echo "compile analyzer"
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/analyzer/GMC
$ROMESYS/bin/romebuilder.exe -i GMC.xml
cd $STANDALONE_INSTALL_DIR

# -------------------
# COMPILER
echo "compile converter"
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/converter
cmake .
make
cd $STANDALONE_INSTALL_DIR

# -------------------
# GEOMETRY
echo "copy geometry GDML/XML files"
cd $STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/analyzer/GMC
cp /afs/cern.ch/work/l/llavezzi/public/geometry/* .
# set the right PATH in the xml files
string1="<SPValue>/afs/cern.ch/user/w/welmeten/public/IDEA/DriftChamberPLUSVertex/simulation/g4GMC/config</SPValue>"
string2="<SPValue>$STANDALONE_INSTALL_DIR/DriftChamberPLUSVertex/simulation/g4GMC/config</SPValue>"
sed -i "s|$string1|$string2|g" ./geant4MC-IDEA.xml
sed -i "s|$string1|$string2|g" ./geant4MC-IDEA-fit.xml

cd $STANDALONE_INSTALL_DIR
echo "finished installation in $STANDALONE_INSTALL_DIR"
