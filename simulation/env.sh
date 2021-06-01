#!/bin/bash
source /cvmfs/sw.hsf.org/key4hep/setup.sh

export PRJBASE="/afs/cern.ch/work/l/llavezzi/IDEA/DRIFT_CH/gitIDEA"

# suggestions by Valentin
for path in ${CMAKE_PREFIX_PATH//:/ }; do
      if  grep  -q 'geant4-[0-9]' <<< "$path"  ; then
          echo "$path"
	  export G4BASE=$path
	  export G4INSTALL=$G4BASE/share/Geant4-10.7.1/geant4make
          export G4LIB=$G4BASE/lib64
          export G4INCLUDE=$G4BASE/include
	  export G4SYSTEM=Linux-g++
      fi
      if  grep -q 'clhep' <<< "$path"  ; then
          echo "$path"
          export CLHEP_BASE_DIR=$path
          export CLHEP_INCLUDE_DIR=$CLHEP_BASE_DIR/include
          export CLHEP_LIB_DIR=$CLHEP_BASE_DIR/lib
      fi
done

# simulation (see README.md)
export SIM_INSTAL_DIR=$PRJBASE/simulation/instal_dir # <installation_directory>
export SIM_OUTPUT_DIR=$PRJBASE/simulation/output_dir # <output_directory>
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SIM_INSTAL_DIR/lib
