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
Working on VI.

To compile simulation:
```
cd simulation/
```
modify the PRJBASE in env.sh according to your installation.
Set a <installation_directory> where you want make install to set the bin, lib, etc.
Set a <output_directory> where you want the output files at runtime to be put.
```
source env.sh
mkdir build <installation_directory> <output_directory>
cd build/
#cmake -DCMAKE_INSTALL_PREFIX=<installation_directory> -DCMAKE_CXX_FLAGS="-DMT_OFF"  ../
cmake -DCMAKE_INSTALL_PREFIX=$SIM_INSTAL_DIR  -DCMAKE_CXX_FLAGS="-DMT_OFF"  ../g4GMC
make
make install
```
To compile analyzer:
```
cd analyzer/
```
modify the PRJBASE in envGMC.sh according to your installation.
Install ROME and set its env in envGMC.sh 
Install GENFIT2 and set its env in envGMC.sh 

```
source envGMC.sh
cd GMC
$ROMESYS/bin/romebuilder.exe -i GMC.xml
```


## Run the simulation
```
#cd <installation_directory> 
cd $SIM_INSTAL_DIR
# ./bin/g4GMC ./g4mac/runPFix-1.mac geom_IDEA.txt 0 <output_directory>
mkdir -p $SIM_OUTPUT_DIR
./bin/g4GMC ./g4mac/runPFix-1.mac geom_IDEA.txt 0 $SIM_OUTPUT_DIR

```
## Converter G4hits to EDM
It is contained in the file convertHits.cc, compiled together with the rest. </br>
!!!FOR NOW!!! (to be done):
- the binary file is inside build/bin, it is not moved to the right place
- the output file is not int the $SIM_OUTPUT_DIR but it is put in the dir where you run the conversion

To run

```
./bin/convertHits $SIM_OUTPUT_DIR/<inout_file.root>
```
