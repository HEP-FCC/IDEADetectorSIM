#!/bin/bash

echo -e "*****  Scripting to generate launch a simulation dived in $1 runs *****\n "

nRuns=$1
nFrstRun=$2
inMacFile=$3
geomFile=$4
outFold=$5

nMaxSeed=200

if [ $nRuns -gt $nMaxSeed ];
then
  echo -e "--> Generate the seeds \n"
  root -b -q GenerateSeeds.C\($nRuns\)
  nMaxSeed=$nRuns
fi
outFold=$PWD/tmp
#rm -fr ${outFold}/tmpG4mac
mkdir -p ${outFold}/tmpG4mac
#rm -f ${outFold}/tmpG4mac/*

filename="SeedsGenerated_${nMaxSeed}.lst"
echo -e "--> The Seed file in use: $filename \n"

irun=0
while [ $irun -lt $nRuns ]; do

    let iRun=$nFrstRun+$irun
    let irun+=1
    let nl=$iRun+1
    line=$(head -$nl $filename | tail -1)

    sleng=`echo ${#line}`

    seedx=`echo ${line:0:9}`
    seedy=`echo ${line:10:9}`

    #echo "${seedx} -- ${seedy}"

    irunMacFile=${outFold}/tmpG4mac/run_${iRun}.mac
    
    cat ${inMacFile} | \
    sed -e 's/[0-9]\{9\}/'$seedx'/1' -e 's/[0-9]\{9\}/'$seedy'/2' > ${irunMacFile}

    nohup ${PRJBASE}/simulation/g4GMC/bin/Linux-g++/g4GMC ${irunMacFile} ${geomFile} ${iRun} ${outFold} >& ${outFold}/sim_${iRun}.log &

done

echo -e " **** Simulation loop is successfully done!!! \n "
echo -e "*************************************************"
