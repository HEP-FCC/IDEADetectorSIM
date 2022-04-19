#!/bin/bash

# suggestions by Valentin to retrieve necessary env for the rest of compilation 
for path in ${CMAKE_PREFIX_PATH//:/ }; do

    # GEANT4 --------------------------------------
    if  grep  -q '/geant4-10.7.1' <<< "$path"  ; then
        echo "$path"
	export G4BASE=$path
	export G4INSTALL=$G4BASE/share/Geant4-10.7.1/geant4make
        export G4LIB=$G4BASE/lib64
        export G4INCLUDE=$G4BASE/include
	export G4SYSTEM=Linux-g++
    fi

    # CLHEP ---------------------------------------
    if  grep -q 'clhep' <<< "$path"  ; then
        echo "$path"
        export CLHEP_BASE_DIR=$path
        export CLHEP_INCLUDE_DIR=$CLHEP_BASE_DIR/include
        export CLHEP_LIB_DIR=$CLHEP_BASE_DIR/lib
    fi

    # GENFIT --------------------------------------

    if  grep -q 'genfit' <<< "$path"  ; then
        export GENFIT=$path
    fi

    ROMESYS=${PWD}/external/rome3
    export PATH=$ROMESYS/bin:$PATH

done


echo "GEANT4 is set to $G4BASE"
echo "CLHEP is set to $CLHEP_BASE_DIR"
echo "GENFIT set to " $GENFIT

