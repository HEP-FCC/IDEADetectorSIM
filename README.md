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
#cmake -DCMAKE_INSTALL_PREFIX=<installation_directory> ../
cmake -DCMAKE_INSTALL_PREFIX=$SIM_INSTAL_DIR ../
make
make install
#cd <installation_directory> 
cd $SIM_INSTAL_DIR
# ./bin/g4GMC ./g4mac/runPFix-1.mac geom_IDEA.txt 0 <output_directory>
mkdir -p $SIM_OUTPUT_DIR
./bin/g4GMC ./g4mac/runPFix-1.mac geom_IDEA.txt 0 $SIM_OUTPUT_DIR

```

