# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(g4GMC_converter)

set(BASE_FLD
	EXCEP
	GEOMSRVC
)

set(DET_FLD
	SVX
	PSHW
	CDCH
	TDCH
	PHCV
	DRFPIC
	MUSPC
)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Find HepMC (required package)
#
find_package(HepMC REQUIRED)

#----------------------------------------------------------------------------
# Find EDM/podio
#
find_package(EDM4HEP)
find_package(podio)
#find_package(DD4hep)

#----------------------------------------------------------------------------
# Find ROOT
#
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO EG)
include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# THIS PROJECT
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#--------------------------------------------------------------------
# headers
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories($ENV{PRJBASE}/simulation/g4GMC/include
		$ENV{PRJBASE}/simulation/g4GMC/GEOMSRV/include
 		$ENV{PRJBASE}/analyzer/GMC/include
                ${Geant4_INCLUDE_DIR}
                ${HEPMC_INCLUDE_DIR}
                $ENV{PRJBASE}/simulation/ConfigReader/include
		$ENV{ROOTSYS}/include/
		$ENV{G4INCLUDE}
		$ENV{ROMESYS}/include/
		$ENV{PRJBASE}/simulation/g4GMC
		$ENV{CLHEP_INCLUDE_DIR}
		$ENV{DD4hepINSTALL}
		$ENV{PRJBASE}/../build/DRDigitization
                ${CMAKE_INSTALL_PREFIX}/include/sipm
		${CMAKE_INSTALL_PREFIX}/include/DdAndCalibration
)

foreach(basefld ${BASE_FLD})
include_directories($ENV{PRJBASE}/simulation/${basefld}/include)
endforeach()

# -------------------------------------
add_executable(readHits readHits.cc)

target_link_libraries(readHits PUBLIC
		G4GMC
		GMCG4ClassesDict
 		GMCAnalyzer
 		stdc++
		SCrd
		${BASE_FLD}
                ${DET_FLD}
                ${Geant4_LIBRARIES}
                ${HEPMC_LIBRARIES}
                ${HEPMC_FIO_LIBRARIES}
                ${PYTHIA6_LIBRARIES}
                ${ROOT_LIBRARIES}
		Core
		Imt
		RIO
		Geom
		Net
		Hist
		Graf
		Graf3d
		Gpad
		ROOTVecOps
		Tree
		TreePlayer
		Rint
		Postscript
		Matrix
		Physics
		Minuit
		Html
		Gui
		MathCore
		Thread
		MultiProc
		ROOTDataFrame
		pthread
		GL
		GLU
		Xrandr
		Xext
		X11
		EDM4HEP::edm4hep
		podio::podioRootIO
		)

target_link_directories(readHits PUBLIC
		$ENV{CLHEP_LIB_DIR}
		$ENV{G4LIB}
		$ENV{SIM_INSTAL_DIR}/lib
		$ENV{PRJBASE}/analyzer/GMC/obj
		$ENV{PRJBASE}/../build/lib
		$ENV{PRJBASE}/../build/lib64
#		$ENV{GENFIT2SYS}/lib
)	      

#--------------------------------------------------------------------
# convertHits.cc
#
add_executable(convertHits convertHits.cc)

target_link_libraries(convertHits PUBLIC
                      G4GMC
                      GMCG4ClassesDict
                      SCrd
                      ${BASE_FLD}
                      ${DET_FLD}
                      ${Geant4_LIBRARIES}
                      ${HEPMC_LIBRARIES}
                      ${HEPMC_FIO_LIBRARIES}
                      ${PYTHIA6_LIBRARIES}
                      ${ROOT_LIBRARIES}
                      gfortran
                      EDM4HEP::edm4hep
                      podio::podioRootIO
                      )

target_link_directories(convertHits PUBLIC
                $ENV{CLHEP_LIB_DIR}
                $ENV{G4LIB}
                $ENV{SIM_INSTAL_DIR}/lib
		$ENV{PRJBASE}/analyzer/GMC/obj
#               $ENV{GENFIT2SYS}/lib
)

#--------------------------------------------------------------------
# convertTracks.cc
#
add_executable(convertTracks convertTracks.cc)

target_link_libraries(convertTracks PUBLIC
	              G4GMC
                      GMCG4ClassesDict
                      SCrd
                      ${BASE_FLD}
                      ${DET_FLD}
                      ${Geant4_LIBRARIES}
                      ${HEPMC_LIBRARIES}
                      ${HEPMC_FIO_LIBRARIES}
                      ${PYTHIA6_LIBRARIES}
                      ${ROOT_LIBRARIES}
             #         ${${PROJECT_NAME}_LIB}
                      gfortran
                     EDM4HEP::edm4hep
                     podio::podioRootIO
                     )

target_link_directories(convertTracks PUBLIC
                $ENV{CLHEP_LIB_DIR}
                $ENV{G4LIB}
		$ENV{SIM_INSTAL_DIR}/lib
                $ENV{PRJBASE}/analyzer/GMC/obj
#               $ENV{GENFIT2SYS}/lib
)

#--------------------------------------------------------------------
# buildRecoLevelIDEAFile.cc
#
add_executable(buildRecoLevelIDEAFile buildRecoLevelIDEAFile.cc)

target_link_libraries(buildRecoLevelIDEAFile PUBLIC
	              G4GMC
                      GMCG4ClassesDict
                      SCrd
                      ${BASE_FLD}
                      ${DET_FLD}
                      ${Geant4_LIBRARIES}
                      ${HEPMC_LIBRARIES}
                      ${HEPMC_FIO_LIBRARIES}
                      ${PYTHIA6_LIBRARIES}
                      ${ROOT_LIBRARIES}
             #         ${${PROJECT_NAME}_LIB}
                      gfortran
                     EDM4HEP::edm4hep
                     podio::podioRootIO
		     DRCalo_DRDigitizationLib 
		     sipm
                     )

target_link_directories(buildRecoLevelIDEAFile PUBLIC
                $ENV{CLHEP_LIB_DIR}
                $ENV{G4LIB}
		$ENV{SIM_INSTAL_DIR}/lib
                $ENV{PRJBASE}/analyzer/GMC/obj
#               $ENV{GENFIT2SYS}/lib
		${CMAKE_INSTALL_PREFIX}/lib64
		${CMAKE_INSTALL_PREFIX}/lib
)


#--------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install (TARGETS readHits DESTINATION bin) 
install (TARGETS convertHits DESTINATION bin) 
install (TARGETS convertTracks DESTINATION bin) 
install (TARGETS buildRecoLevelIDEAFile DESTINATION bin) 


