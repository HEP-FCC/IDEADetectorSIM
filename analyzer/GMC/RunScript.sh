#!/bin/sh

#echo "script to run a short data analysis \n"

#./gmcanalyzer.exe -b -i geant4MC.xml -r 2 -e 1-200 > file.log
 
#./gmcanalyzer.exe -I -b -q ../macros/PeakAnalysis_v1.C > file1.log

./gmcanalyzer.exe -I -b -q ../macros/ClaudioClusterRagr_new.C\(20000\) > pippo.log

mv PlotGenerati.root ~/html/allow_listing/.
mv data_file.txt ~/html/allow_listing/.


echo "end script \n"



