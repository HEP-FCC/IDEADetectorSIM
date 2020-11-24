#!/bin/sh

echo -e "*****  Scripting to convert MC hits in $1 runs *****\n "

nRuns=$1
nFrstRun=$2

nl=0
while [ $nl -lt $nRuns ]; do

    let iRun=$nFrstRun+$nl
    let nl+=1

    printf -v hitsFlNum "%05d" $iRun
#    echo "hitsFlNum ${hitsFlNum}"
   
    nohup ${PRJBASE}/simulation/g4GMC/bin/Linux-g++/readHits hits${hitsFlNum}.root >& rdhits_${iRun}.log &

done

echo -e " **** Conversion loop is successfully done!!! \n "
echo -e "*************************************************"
