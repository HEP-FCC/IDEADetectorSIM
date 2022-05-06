#!/bin/bash


# suggestions by Valentin to retrieve necessary env for the rest of compilation 
for path in ${CMAKE_PREFIX_PATH//:/ }; do

    # GEANT4 --------------------------------------
    if  grep  -q '/geant4-10.7.1' <<< "$path"  ; then
	export G4BASE=$path
	export G4INSTALL=$G4BASE/share/Geant4-10.7.1/geant4make
        export G4LIB=$G4BASE/lib64
        export G4INCLUDE=$G4BASE/include
	export G4SYSTEM=Linux-g++
    fi

    # CLHEP ---------------------------------------
    if  grep -q 'clhep' <<< "$path"  ; then
        export CLHEP_BASE_DIR=$path
        export CLHEP_INCLUDE_DIR=$CLHEP_BASE_DIR/include
        export CLHEP_LIB_DIR=$CLHEP_BASE_DIR/lib
    fi

    # GENFIT --------------------------------------

    if  grep -q 'genfit' <<< "$path"  ; then
        export GENFIT=$path
    fi

    if  grep -q 'zlib' <<< "$path"  ; then
	export ZLIB_DIR=$path/lib
    fi

    if grep -q 'eigen' <<< "$path" ; then
	export EIGEN3SYS=$path
    fi

done

MYG4LIB=/its/home/iv41/workarea/FullIDEASim_improveSetup2/build/instal_dir/lib

if [[ $MYG4LIB == *CMAKE_INSTALL_PREFIX* ]]
then
    echo 'Check whether you can compile the simulation now'
else
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${MYG4LIB}
fi



ROMESYS=/its/home/iv41/workarea/testrome/DriftChamberPLUSVertex/LOCAL/rome

if [[ $ROMESYS == *ROME_INST_DIR* ]]
then
    echo 'ROMESYS will be configured after installation'
else
    echo "ROMESYS is set to " ${ROMESYS}
    export PATH=$ROMESYS/bin:$PATH
fi



#needed by the analyzer
############################
GENFIT2SYS=${GENFIT}
if [ ! -d "${GENFIT2SYS}/lib64" ] 
then
    echo "WARNING! " ${GENFIT2SYS}/lib64 " does not exist"
fi
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${GENFIT2SYS}/lib64
export ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${GENFIT2SYS}/include

export ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${EIGEN3SYS}/include/eigen3

export GMCDIR=/its/home/iv41/workarea/testrome/DriftChamberPLUSVertex/analyzer/GMC

if [[ $GMCDIR == *GMC_INST_DIR* ]]
then
    echo 'READY to compile GMC analyzer'
else
    echo  "GMCDIR is set to "${GMCDIR}
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${GMCDIR}/obj
fi

export ROT
export LIBROME=yes

echo "GEANT4 is set to $G4BASE"
echo "CLHEP is set to $CLHEP_BASE_DIR"
echo "GENFIT set to " $GENFIT
echo "EIGEN3SYS is set to "${EIGEN3SYS}
echo  "ZLIB_DIR is set to "${ZLIB_DIR}
