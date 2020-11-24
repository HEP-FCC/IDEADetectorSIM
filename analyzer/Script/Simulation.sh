#!/bin/sh

echo "**** script to run 1000 event @ three different BandWidth ****** \n"


cd /data/chiric/CodeCommitted/analyzer

./envGMC.sh

cd GMC

#cp geant4MC_300MHz.xml geant4MC.xml

for irun in `seq 23 52` ;
do
./gmcanalyzer.exe -b -q -i geant4MC_800MHz.xml -r $irun -e 1-1000 > file.log

fname=`printf "MCHits%05i.root" $irun`
finalFile=`printf MCHits800MHz_g4_SN7_2GSa_%03i.root $irun`

mv $fname ../Simdata/$finalFile

done

#./gmcanalyzer.exe -b -q -I RunClusterize.C\(1\) > file.log 

#./gmcanalyzer.exe -b -q -I RunClusterize.C\(2\) > file.log 

#mv EfficiencyPlot.root EfficiencyPlotFirst.root

#./gmcanalyzer.exe -b -q -I RunClusterize.C\(3\) > file.log

#mv EfficiencyPlot.root EfficiencyPlotMedie.root

./gmcanalyzer.exe -b -q -I RunClusterize.C\(4\) > file.log
exit

