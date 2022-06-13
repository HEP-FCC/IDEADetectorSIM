#! /bin/bash 

###### To be run in the build directory

if [ $# -lt 3 ] 
then
    echo "USAGE: "${0}" [INPUT ID HITS] [INPUT CALO HITS] [OUTPUT FILE NAME]"
    return  
fi 


INPUTFILETRACKHITS=$1
INPUTFILECALO=$2
OUTPUTFILE=$3

if [ ! -f ${INPUTFILETRACKHITS} ] 
then 
    echo "File " ${INPUTFILETRACKHITS} " cannot be found. Exiting."
    return 
fi

if [ ! -f ${INPUTFILECALO} ] 
then 
    echo "File " ${INPUTFILECALO} " cannot be found. Exiting."
    return  
fi

shift 3

readHits ${INPUTFILETRACKHITS}
gmcanalyzer.exe -b -q -i geant4MC-IDEA.xml  -r 1 >& out-hits_1.log
gmcanalyzer.exe -b -q -i geant4MC-IDEA-fit.xml  -r 1 >& out-reco_1.log
buildRecoLevelIDEAFile RecoData00001.root ${INPUTFILECALO}

