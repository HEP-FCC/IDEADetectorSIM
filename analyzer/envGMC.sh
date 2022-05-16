#!/bin/bash

#  $Id: envGMC.sh 268 2013-10-31 17:21:49Z tassielli $
#  $Author:  $
#  $Revision:  $
#
# modified for porting to FCCSW/Key4Hep by lavezzi (2020)


export PRJBASE="/afs/cern.ch/work/w/welmeten/public/IDEA_calo_Key4HEP/DriftChamberPLUSVertex"
export PATH=$PRJBASE/analyzer/GMC:$PATH

# genfit2
export GENFIT2SYS=/cvmfs/sw.hsf.org/spackages4/genfit/02-00-00/x86_64-centos7-gcc8.3.0-opt/l7zbfli
export LD_LIBRARY_PATH=${GENFIT2SYS}/lib64:${LD_LIBRARY_PATH}
export ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${GENFIT2SYS}/include
echo "GENFIT2SYS is set to "${GENFIT2SYS}

# eigen
export EIGEN3SYS=/usr
export ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${EIGEN3SYS}/include/eigen3
echo "EIGEN3SYS is set to "${EIGEN3SYS}

# rome
export ROMESYS=/afs/cern.ch/work/w/welmeten/public/IDEA_calo_Key4HEP/LOCAL/ROME/rome
export PATH=$ROMESYS/bin:${PATH}
export LIBROME=yes
echo "ROME is set to "${ROMESYS}

#GMC
export GMCDIR=${PRJBASE}/analyzer/GMC
export LD_LIBRARY_PATH=${GMCDIR}/obj:${LD_LIBRARY_PATH}
echo "GMCDIR is set to "${GMCDIR}

#root
export ROOTSYS=path_to_root
export ROT

#zlib
for path in ${CMAKE_PREFIX_PATH//:/ }; do
    if  grep -q 'zlib' <<< "$path"  ; then
    export ZLIB_DIR=$path/lib
    fi
done
echo "ZLIB_DIR is set to "${ZLIB_DIR}


export G4WORKDIR=$SIM_INSTAL_DIR
export LD_LIBRARY_PATH=${G4WORKDIR}/lib:${LD_LIBRARY_PATH}

