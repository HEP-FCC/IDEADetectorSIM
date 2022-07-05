## Quick start
$ Code originally developed by Wenxing Fang for CEPC here: git clone https://github.com/key4hep/k4Pandora.git

```
$ cd k4Pandora
$ source my_setup.sh
$ mkdir build && cd build
$ cmake .. 
$ make
$ ./run gaudirun.py ../Examples/options/tut_pandora.py
```

$ Input: simulation code from Iacopo Vivarelle where there are (edm4hep collections of) hits divided into S_ and C_ categories (to be read in the .py file)
$ Main code: PandoraSCAlg.cpp: handles calorimeter hit collections and modifies the hit type according to the S/C info
$ To do: implement a dumper algorithm that dumps a text file with the relevant hit info, which will feed our NN algorithm



### Some Notices
* k4Pandora is a pandora app for the Key4HEP software framework. It uses Gaudi_v35 framework for running and Edm4hep for the event data model.  

* Configuration of pandora algorithm is set by pandoralg in tut_pandora.py. 
* Function to get ClusterShapes (in PfoCreator.cpp) of a cluster is still from Marlin.
