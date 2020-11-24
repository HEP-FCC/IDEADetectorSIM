#!/bin/sh

echo -e "*****  Scripting to generate $1 data files of simulated events *****\n "

echo -e "--> Generate the seeds \n"
root -b -q GenerateSeeds.C\($1\) > file.log

rm file.log

mkdir -p ../SimData
nfiledir=`ls ../SimData/MCData*.root | wc -l`

filename="SeedsGenerated.txt"
echo -e "--> The file: $filename has been generated \n"

echo -e " **** Starting simulation loop **** \n"
nlines=$(wc -l $filename | awk '{ print $1 }')

nl=0
idfile=0
while [ $nl -lt $nlines ]; do

    let nl+=1
    line=$(head -$nl $filename | tail -1)

    sleng=`echo ${#line}`

    seedx=`echo ${line:0:9}`
    npos=`expr $sleng-9`
    seedy=`echo ${line:$npos:9}`

    cat runTest.mac | \
	sed -e 's/xxx/'$seedx'/' -e 's/yyy/'$seedy'/' > run2.mac

    echo -e "--> call g4GMC with seeds: $seedx, $seedy \n"
    ./bin/Linux-g++/g4GMC run2.mac > file.log
    rm file.log

    echo -e "--> Convert the file hits.root in MCData tree \n"
    ./bin/Linux-g++/readHits hits.root > file.log
    rm file.log

    let idfile=nfiledir+nl
    outfileroot="MCData"$(printf "%05d" $idfile)".root"
    mv MCData00001.root ../SimData/$outfileroot
    echo -e "--> The file $outfileroot was generated in ../SimData \n"

    rm hits.root
done

echo -e " **** Simulation loop is successfully done!!! \n "
echo -e "*************************************************"
