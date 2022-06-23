# CHANGE LOG

**On 2021-12-05, restored compatibility with latest key4hep stack** </br>
On 2022-05-06, Developed a full cmake configuration. </br>
On 2021-12-05, rome moved from tagged branch to master branch </br>
On 2021-12-03, genfit2 installationon on the stack is used (before it was locally installed) </br>
<s>On 2021-12-01 I forced the key4hep stack to be used to 2021-09-01 as with the latest version it does not work</s> </br>
On 2021-05, since the master analyzer does not compile I moved to Gianfranco's version of the code </br>


## RECOMMENDED use of the package

Instructions:

### Building the package using cmake directly

Use this repository as any other git repositories                                                       

<ul>                                                                                                                
   <li> Fork and clone the repository. Check out the branch of interest. Say the repository is in /my/dir/ </li>    
   <li> Prepare your environment. </li>                                                                             
                                                                                                                    
   ```                                                                                                              
   cd /my/dir/IDEADetectorSIM                                                                                
   source key4hep_setup.sh                                                                                          
   source env.sh                                                                                                    
   ```                                                                                                              
   <li> Make a build directory at the same level as the repository and cd into it. </li> 
                           
                                                                                                                    
   ```                                                                                                              
      mkdir /my/dir/build                                                                                           
      cd /my/dir/build                                                                                              
   ```                                                                                                              
                                                                                                                    
   <li> Configure and compile: </li>                                                                                
                                                                                                                    
   ```                                                                                                              
      cmake -DCMAKE_INSTALL_PREFIX=/my/dir/build/instal_dir -DCMAKE_CXX_FLAGS="-DMT_OFF" ../IDEADetectorSIM  
      make -j10                                                                                                 
      make install                                                                                                  
   ```                                                                                                              
   <li>  Sit back and enjoy a coffee. Assuming everything goes well, the compilation time is easily in the tens of m
inutes.</li>                                                                                                        
   <li> Now there is a file env.sh in the build directory </li>                                            
         
                                                                                                                    
   ```                                                                                                              
   source env.sh                                                                                                    
   ```                                                                                                              
                                                                                                                    
   <li> You should be ready to run.</li>                                                                           

### Using the package (once it is built)
 
   <li> Every time you log in again, you should do: </li>                                                  
                                                                                                                    
   ```                                                                                                              
   cd /my/dir/IDEADetectorSIM                                                                                
   source key4hep_setup.sh                                                                                          
   cd ../build                                                                                                      
   source env.sh                                                                                                    
   ```            
   Note that the env.sh to be used is the one in the build directory.

### Building the package using the install_standalone script

<ul>
   <li> Create an empty directory, say called mydir. cd into it and clone the repository. </li>
   <li> Clone the package and checkout the master branch</li>
   <li> Copy the install_standalone.sh script in mydir </li> 
   <li> Edit it and set STANDALONE_INSTALL_DIR to the directory where you want to install everything </li>
   <li> Execute it with: source ./install_standalone.sh </li>
</ul>

Go take a coffee. The code should compile and be ready to be used. If you do not have access to afs, you will seee an error related to copy a gdml file, you will have to copy that by hand. 

Every time you log in, you will have to source simulation/env.sh and analyzer/envGMC.sh. 
   
</ul>      

## Run the simulation 

You may find useful to define SIM_INSTAL_DIR as the simulation installation directory. This is done automatically with the install_standalone script. The simulation is installed directly in the build directory if you build teh packahe directly using cmake. Another useful variable to define (automatic with the install_standalone script) is SIM_OUTPUT_DIR (where the output of the simulation will be written). 

IMPORTANT: run this in directory $SIM_INSTAL_DIR </br>

```
cd $SIM_INSTAL_DIR
mkdir -p $SIM_OUTPUT_DIR
./bin/g4GMC ./g4mac/runPFix-1.mac geom_IDEA.txt 1 $SIM_OUTPUT_DIR
```
The parameter 1 above is just the runnumber. It can be any number, we assume it is 1 in teh followin. If everything goes smooth, there shoudl be two files in teh $SIM_OUTPUT_DIR directory, one (big) called hits00001.root containing the inner detector hits, and one called simhits_podio00001.root.

## Producing a podio file containing tracks and sigitised calorimeter hits

If you have built the package with cmake, you can simply go to your build directory and do

```
source ./scripts/commonRecoIDEAFile.sh $SIM_OUTPUT_DIR/hits00001.root $SIM_OUTPUT_DIR/simhits_podio00001.root [EDM_OUTPUT_FILE]
```
where [EDM_OUTPUT_FILE] can be anything. If everything runs smooth, you should end up having a file called [EDM_OUTPUT_FILE] in the build directory.


## If you used the install_standalone script

### Conversion from G4 based hit to ROME readable hit

The readHits executable will be in the converter directory.

```
./readHits $SIM_OUTPUT_DIR/<inout_file.root>
```
(the output file is named form MCData00001.root)

### Run the ROME reconstruction
IMPORTANT: run all this in directory analyzer/GMC  </br>

copy the output file from previous step, of the form MCData00001.root, from converter/ to analyzer/GMC </br>

copy the xml/gdml files from /afs/cern.ch/work/l/llavezzi/public/geometry to analyzer/GMC and change the path in the two xml files (gdml is good as it is) </br>

run:
```
./LaunchAnalyzer.sh 1 1 geant4MC-IDEA.xml hits
./LaunchAnalyzer.sh 1 1 geant4MC-IDEA-fit.xml reco
```

### Converter from GMCRecoTrack to Track
IMPORTANT: run this in directory converter/  </br>

copy the output file from previous step, of the form RecoData00001.root, analyzer/GMC to $SIM_OUTPUT_DIR </br>

```
./convertTracks $SIM_OUTPUT_DIR/<reco_data_file.root>
```

### Converter G4hits to EDM
will be ported to directory converter/ </br>

it is contained in the file convertHits.cc, compiled together with the rest. </br>
!!!FOR NOW!!! (to be done):
- the binary file is inside build/bin, it is not moved to the right place
- the output file is not int the $SIM_OUTPUT_DIR but it is put in the dir where you run the conversion

to run

```
./convertHits $SIM_OUTPUT_DIR/<inout_file.root>
```

## Everything is working with these versions
<ul>
   <li> key4hep-stack/2021-11-26:
   <li> gcc8.3.0
   <li> geant4-10.7.2
   <li> clhep-2.4.4.0
   <li> root-6.24.06
   <li> genfit/02-00-00 (on the stack)
   <li> rome master
</ul>

