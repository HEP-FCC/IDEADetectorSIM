# THIS SPECIFIC VERSION OF THE CODE
It is devoted to move:
<ul>
   <li> from env set by hand to using the key4hep stack </li>
   <li> from GNUmakefile to CMakeList.txt </li>
   <li> from standalone hits to EDM hits </li>
</ul>
Working on II; for the moment, only simulation is considered.

To compile:
```
cd simulation/
```
modify the PRJBASE in env.sh according to your installation.
Set a <installation_directory> where you want make install to set the bin, lib, ecc.
Set a <output_directory> where you want the output files at runtime to be put.
```
source env.sh
cd g4GMC/
mkdir build
cd build/
#cmake -DCMAKE_INSTALL_PREFIX=<installation_directory> -DCMAKE_CXX_FLAGS="-DMT_OFF"  ../
cmake -DCMAKE_INSTALL_PREFIX=$SIM_INSTAL_DIR  -DCMAKE_CXX_FLAGS="-DMT_OFF"  ../
make
make install
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
