#!/bin/sh

echo -e "*****  Scripting to run anylazer jobs on $1 runs *****\n "

nRuns=$1
nFrstRun=$2
xmlScript=$3
fix=$4

nl=0
while [ $nl -lt $nRuns ]; do

    let iRun=$nFrstRun+$nl
    let nl+=1
   
    nohup ${PRJBASE}/analyzer/GMC/gmcanalyzer.exe -b -q -i ${xmlScript} -r ${iRun} >& out-${fix}_${iRun}.log &

done

echo -e " **** Jobs loop is successfully done!!! \n "
echo -e "*************************************************"
