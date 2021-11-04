#!/bin/bash

#  $Id: envGMC.sh 268 2013-10-31 17:21:49Z tassielli $
#  $Author:  $
#  $Revision:  $

# base PRJ
if [ -v PRJBASE ];
then
	echo "PRJ BASE already set to "${PRJBASE}
else
	export PRJBASE="/afs/cern.ch/work/l/llavezzi/IDEA/DRIFT_CH/gitIDEA"
fi
export PATH=$PRJBASE/analyzer/GMC:$PATH

#gcc xml
#export GCCXMLPATH=${GCCXML_FQ_DIR}/bin

#GenFit
echo "GENFIT2SYS already set to "${GENFIT2SYS}
export GENFIT2SYS=/afs/cern.ch/work/l/llavezzi/IDEA/LOCAL/GENFIT/master20191106/build
export LD_LIBRARY_PATH=${GENFIT2SYS}/lib64:${LD_LIBRARY_PATH}
if [ -n "$ROOT_INCLUDE_PATH" ] ; then
    export ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${GENFIT2SYS}/include
else
    export ROOT_INCLUDE_PATH=${GENFIT2SYS}/include
fi
#Eigen
export EIGEN3SYS=/usr
export ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${EIGEN3SYS}/include/eigen3
echo "GENFIT2SYS is set to "${GENFIT2SYS}

#geant4
#source /data/softlib/geant/geant41002p01_gcc531/share/Geant4-10.2.1/geant4make/geant4make.sh

#rome
#	export ROMESYS=/mnt/c/workSpace/sw/ROME/rome-master20180921
export ROMESYS=/afs/cern.ch/work/l/llavezzi/IDEA/LOCAL/ROME/rome-v3.2.15.1
export PATH=$ROMESYS/bin:${PATH}
export LIBROME=yes
echo "ROME is set to "${ROMESYS}

#GMC
export GMCDIR=${PRJBASE}/analyzer/GMC
export LD_LIBRARY_PATH=${GMCDIR}/obj:${LD_LIBRARY_PATH}
echo "GMCDIR is set to "${GMCDIR}

#root
if [ -v ROOTSYS ]
then
        echo "ROOT already set to "${ROOTSYS}
else
	source /afs/cern.ch/work/w/welmeten/public/IDEA-sw/ROOT/6.14.06/install/bin/thisroot.sh
#       export ROOTSYS=/pro/root_v5_34_30
#       export PATH=$ROOTSYS/bin:${PATH}
#       export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH}
fi

#midas
#export MIDASSYS=/mnt/sndhd/sw/meg/midas
#export MIDAS_DIR=/mnt/sndhd/sw/meg/midas
#export MIDAS_EXPTAB=$MIDAS_DIR/exptab
#export MIDAS_EXPT_NAME=FIRB_DAQ
#export PATH=${MIDASSYS}/linux/bin:$PATH
#export LD_LIBRARY_PATH=${MIDASSYS}/linux/lib:${LD_LIBRARY_PATH}

export G4WORKDIR=$SIM_INSTAL_DIR
export LD_LIBRARY_PATH=${G4WORKDIR}/lib:${LD_LIBRARY_PATH}

