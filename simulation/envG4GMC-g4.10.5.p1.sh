#!/bin/bash

#  $Id: envGMC.sh 268 2013-10-31 17:21:49Z tassielli $
#  $Author:  $
#  $Revision:  $

# base PRJ
if [ -v PRJBASE ];
then
	echo "PRJ BASE already set to "${PRJBASE}
else
	export PRJBASE="/afs/cern.ch/work/l/llavezzi/IDEA/DRIFT_CH/wIDEA/DriftChamberPLUSVertex"
fi
export PATH=$PRJBASE/simulation/g4GMC:$PATH

#gcc xml

#geant4
#export G4INSTBASE=/cvmfs/geant4.cern.ch/geant4/10.4.p02/x86_64-centos7-gcc63-opt
#cd $G4INSTBASE/share/Geant4-10.4.2/geant4make
source /cvmfs/sft.cern.ch/lcg/contrib/gcc/6.3.0/x86_64-centos7/setup.sh   #add by Walaa
#source /cvmfs/cms.cern.ch/slc7_amd64_gcc820/external/cmake/3.10.2/etc/profile.d/init.sh   #add by Walaa
export G4INSTBASE=/afs/cern.ch/work/w/welmeten/public/IDEA-sw/Geant4/10.5.p01/install
cd $G4INSTBASE/share/Geant4-10.5.1/geant4make
source geant4make.sh
cd -
export CLHEP_VERSION=2.4.1.0
#Fix for missing deps of geant4
export LD_LIBRARY_PATH=$XERCESCROOT/lib:$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH=/afs/cern.ch/work/g/gtassiel/public/sw/tmp-libs/hdf5/1.8.16/lib64:$LD_LIBRARY_PATH
##export LD_LIBRARY_PATH=/afs/cern.ch/work/g/gtassiel/public/sw/tmp-libs/xerces-c/3.1.1/lib64:$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH=/cvmfs/geant4.cern.ch/externals/XercesC/3.1.1/x86_64-cc7-native-opt/lib:$LD_LIBRARY_PATH

#set the workdir for g4GMC
#export homedir=`pwd`
export G4WORKDIR=${PRJBASE}/simulation/g4GMC
export LD_LIBRARY_PATH=${G4WORKDIR}/lib:${PRJBASE}/simulation/ConfigReader/lib:${LD_LIBRARY_PATH}

#rome
if [ -v ROMESYS ]
then
	echo "ROME already set to "${ROMESYS}
else
	export ROMESYS=/afs/cern.ch/work/w/welmeten/public/IDEA-sw/ROME/rome3-master20191106
	export PATH=$ROMESYS/bin:${PATH}
	export LIBROME=yes
fi

#root
if [ -v ROOTSYS ]
then
        echo "ROOT already set to "${ROOTSYS}
else
	source /afs/cern.ch/work/w/welmeten/public/IDEA-sw/ROOT/6.14.06/install/bin/thisroot.sh
#	export ROOTSYS=/pro/root_v5_34_30
#	export PATH=$ROOTSYS/bin:${PATH}
#	export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH}
fi

#midas
#export MIDASSYS=/mnt/sndhd/sw/meg/midas
#export MIDAS_DIR=/mnt/sndhd/sw/meg/midas
#export MIDAS_EXPTAB=$MIDAS_DIR/exptab
#export MIDAS_EXPT_NAME=FIRB_DAQ
#export PATH=${MIDASSYS}/linux/bin:$PATH
#export LD_LIBRARY_PATH=${MIDASSYS}/linux/lib:${LD_LIBRARY_PATH}

#GMC
if [ -v GMCDIR ]
then
	echo "GMCDIR already set to "${GMCDIR}
else
	export GMCDIR=${PRJBASE}/analyzer/GMC
	export LD_LIBRARY_PATH=${GMCDIR}/obj:${LD_LIBRARY_PATH}
fi
