# THIS SPECIFIC VERSION OF THE CODE
Since the master analyzer does not compile I moved to Gianfranco's version of the code (May 2021)

It is devoted to move:
<ul>
   <li> from env set by hand to using the key4hep stack </li>
   <li> simulation: from GNUmakefile to CMakeList.txt </li>
   <li> simulation: from standalone hits to EDM hits </li>
   <li> analyzer: adapt env to new installation of simulation </li>
   <li> analyzer: need external ROME, GENFIT2 </li>
   <li> analyzer: from standalone tracks to EDM tracks </li>
</ul>

## External software
The reconstruction needs external ROME and GENFIT2. They are installed in the directory:
/afs/cern.ch/work/l/llavezzi/public/LOCAL
and you can either use them from your directory or copy and reinstall them elsewhere.  
In order to use them from my directory, set the following variables in the file DriftChamberPLUSVertex/analyzer/envGMC.sh
```
export GENFIT2SYS=/afs/cern.ch/work/l/llavezzi/public/LOCAL/GENFIT/master20191106/build
export ROMESYS=/afs/cern.ch/work/l/llavezzi/public/LOCAL/ROME/rome-v3.2.15.1
```
In order to install them locally copy and compile them (if you want to install it from git check the installation howto on the website)
### GENFIT
```
cp -r /afs/cern.ch/work/l/llavezzi/public/LOCAL/GENFIT dir_where_you_want_your_genfit
cd dir_where_you_want_your_genfit/master20191106
rm -fr build
mkdir build
cd GenFit
(set the GENFIT environmental inside env.sh to dir_where_you_want_your_genfit/master20191106/GenFit)
source env.sh
cd ../build
cmake ../GenFit
make
```
### ROME
Instruction on istallation are on https://bitbucket.org/muegamma/rome3/wiki/Download

```
export ROMESYS=/directory/where/you/have/rome
export PATH=$ROMESYS/bin:$PATH
cd $ROMESYS
make
```
## Download the code
To download the most updated version of the code
```
git clone https://github.com/lialavezzi/DriftChamberPLUSVertex.git
cd DriftChamberPLUSVertex/
git checkout uptodate
```
you will need to compile three directories: simulation, analyzer and converter.

## Compile the code
### To compile simulation:

```
cd simulation/
```

modify env.sh to set:
<ul>
   <li> PRJBASE according to your installation </li>
   <li> SIM_INSTAL_DIR to set the <installation_directory> where you want make install to set the bin, lib, etc. </li>
   <li> SIM_OUTPUT_DIR to set the <output_directory> where you want the output files at runtime to be put </li>
</ul>

then run
```
source env.sh
mkdir build <installation_directory> <output_directory>
cd build/
#cmake -DCMAKE_INSTALL_PREFIX=<installation_directory> -DCMAKE_CXX_FLAGS="-DMT_OFF"  ../
cmake -DCMAKE_INSTALL_PREFIX=$SIM_INSTAL_DIR  -DCMAKE_CXX_FLAGS="-DMT_OFF"  ../g4GMC
make
make install
```

### To compile analyzer:

Install ROME and GENFIT2.

```
cd analyzer/
```

modify envGMC.sh to set
<ul>
   <li> PRJBASE according to your installation </li>
   <li> ROMESYS according to ROME installation </li>
   <li> GENFIT2SYS according to GENFIT2 installation </li>
</ul>

then run
```
source envGMC.sh
cd GMC
$ROMESYS/bin/romebuilder.exe -i GMC.xml
```

### To compile converter (!! compile only after simulation and analyzer !!):
```
cd converter
cmake .
make
```
</br>
Everything is compiled, you can run the simulation and then the reconstruction.

## Run the simulation
IMPORTANT: run this in directory $SIM_INSTAL_DIR </br>

```
#cd <installation_directory> 
cd $SIM_INSTAL_DIR
# ./bin/g4GMC ./g4mac/runPFix-1.mac geom_IDEA.txt 1 <output_directory>
mkdir -p $SIM_OUTPUT_DIR
./bin/g4GMC ./g4mac/runPFix-1.mac geom_IDEA.txt 1 $SIM_OUTPUT_DIR
```

## Conversion from G4 based hit to ROME readable hit
IMPORTANT: run this in directory converter/  </br>

```
./readHits $SIM_OUTPUT_DIR/<inout_file.root>
```
(the output file is in the form MCData00001.root)

## Run the ROME reconstruction
IMPORTANT: run all this in directory analyzer/GMC  </br>

copy the output file from previous step, of the form MCData00001.root, from converter/ to analyzer/GMC </br>

copy the xml/gdml files from /afs/cern.ch/work/l/llavezzi/public/geometry to analyzer/GMC and change the path in the two xml files (gdml is good as it is) </br>

run:
```
./LaunchAnalyzer.sh 1 1 geant4MC-IDEA.xml hits
./LaunchAnalyzer.sh 1 1 geant4MC-IDEA-fit.xml reco
```

## Converter from GMCRecoTrack to Track
IMPORTANT: run this in directory converter/  </br>

```
./convertTracks $SIM_OUTPUT_DIR/<reco_data_file.root>
```

## Converter G4hits to EDM
will be ported to directory converter/ </br>

it is contained in the file convertHits.cc, compiled together with the rest. </br>
!!!FOR NOW!!! (to be done):
- the binary file is inside build/bin, it is not moved to the right place
- the output file is not int the $SIM_OUTPUT_DIR but it is put in the dir where you run the conversion

to run

```
./bin/convertHits $SIM_OUTPUT_DIR/<inout_file.root>
```

## Everything is working with these versions
<ul>
   <li> key4hep-stack/2021-09-01:
   <li> gcc8.3.0
   <li> geant4-10.7.1
   <li> clhep-2.4.4.0
   <li> root-6.24.00
   <li> genfit master2019110
   <li> rome-v3.2.15.1
</ul>

# Open Questions
<ul>
   <li> 
   In convertTracks.cc state vector size != hitlist size (from ROME):
   ERROR, stateVector has size 40 and hitlist has size 52
</ul>

# Reminder for me (Lia)
LA COSA IMPORTANTE E' CHE LA RECO DI ROME SIA RUNNATA NELLA DIRECTORY DOVE C'E' g4-IDEA_reco.gdml, perche' altrimenti (come qui sotto) non funziona!!
- standalone analysis, from $SIM_OUTPUT_DIR, run
$PRJBASE/converter/readHits $SIM_OUTPUT_DIR/hits00001.root
- copia gli xml/gdml files dentro ad analyzer/GMC
e correggi il path
- analizza, da $SIM_OUTPUT_DIR
${PRJBASE}/analyzer/GMC/LaunchAnalyzer.sh 1 1 ${PRJBASE}/analyzer/GMC/geant4MC-IDEA.xml hits
${PRJBASE}/analyzer/GMC/LaunchAnalyzer.sh 1 1 /${PRJBASE}/analyzer/GMC/geant4MC-IDEA-fit.xml reco
