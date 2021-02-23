#!/bin/bash
source /cvmfs/sw.hsf.org/key4hep/setup.sh

export PRJBASE="/afs/cern.ch/work/l/llavezzi/IDEA/DRIFT_CH/wIDEA/DriftChamberPLUSVertex"

# simulation (see README.md)
export SIM_INSTAL_DIR=$PRJBASE/simulation/instal_dir # <installation_directory>
export SIM_OUTPUT_DIR=$PRJBASE/simulation/output_dir # <output_directory>
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SIM_INSTAL_DIR/lib
