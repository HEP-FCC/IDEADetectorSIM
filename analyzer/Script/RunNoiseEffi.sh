#!/bin/sh

echo "**** script to run 1000 event @ three different BandWidth ****** \n"


cd /data/chiric/CodeCommitted/analyzer/GMC

pwd

rpdir="../Simdata"
hdir="../HistRoot"
stroffile="k"
for i in `seq 1 10`
 do
  stroffile=$(printf "%s-%d" $stroffile $i)
  #cambio il valore con cui genero il noise
#  cat geant4MC_1000MHz.xml | \
  cat geant4MC_620MHz.xml | \
      sed -e 's/XXXXX/'$i'/' > geant4MC.xml

  #genera gli eventi
  ./gmcanalyzer.exe -b -q -i geant4MC.xml -r 1 -e 1-1000 > file.log

  #rinomino il file root e lo sposto nella directory
  flname=$(printf "MCHits620MHz_g5_Noise%d_01.root" $i)
  mv MCHits00001.root $flname  
  
  #lancio il peakanalysis
  cp $flname $rpdir/.
  ./gmcanalyzer.exe -b -q -I RunMacros.C\(\"$flname\"\)
  
  flroot=$(printf "PeakAnalysisAt620MHz_BinW400ps_Noise%d.root" $i)
  mv PeakAnalysisAt620MHz_BinW400ps.root $hdir/$flroot
  rm $flname
  
 done

#lancio la macro per produrre il plots
stroffile=`echo ${stroffile:2}`
./gmcanalyzer.exe -b -q -I RunMacros.C\(\"$stroffile\",1\)

exit

