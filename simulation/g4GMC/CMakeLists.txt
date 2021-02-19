# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(g4GMC_simulation)

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
#include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Find HepMC (required package)
#
find_package(HepMC REQUIRED)
#find_package(EDM4HEP)
#find_package(podio)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO EG)
include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR}
                    ${HEPMC_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}/../ConfigReader/include
                    ${PROJECT_SOURCE_DIR}/EXCEP/include
                    ${PROJECT_SOURCE_DIR}/GEOMSRVC/include
#                    ${PROJECT_SOURCE_DIR}/SVX/include
#                    ${PROJECT_SOURCE_DIR}/PSHW/include
#                    ${PROJECT_SOURCE_DIR}/CDCH/include
#                    ${PROJECT_SOURCE_DIR}/TDCH/include
#                    ${PROJECT_SOURCE_DIR}/PHCV/include
#                    ${PROJECT_SOURCE_DIR}/DRFPIC/include
		    )


file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
#	  	  ${PROJECT_SOURCE_DIR}/../ConfigReader/src/*.cc
#	 	  ${PROJECT_SOURCE_DIR}/PSHW/src/*.cc
#                 ${PROJECT_SOURCE_DIR}/SVX/src/*.cc
#                 ${PROJECT_SOURCE_DIR}/EXCEP/src/*.cc
#                 ${PROJECT_SOURCE_DIR}/GEOMSRVC/src/*.cc
#                 ${PROJECT_SOURCE_DIR}/CDCH/src/*.cc)

#file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh
#                  ${PROJECT_SOURCE_DIR}/../ConfigReader/include
#                  ${PROJECT_SOURCE_DIR}/EXCEP/include/*.hh
#	  	   ${PROJECT_SOURCE_DIR}/GEOMSRVC/include/*.hh
#                  ${PROJECT_SOURCE_DIR}/SVX/include/*.hh
#                  ${PROJECT_SOURCE_DIR}/PSHW/include/*.hh
#                  ${PROJECT_SOURCE_DIR}/CDCH/include/*.hh
#                  ${PROJECT_SOURCE_DIR}/TDCH/include/*.hh
#                  ${PROJECT_SOURCE_DIR}/PHCV/include/*.hh
#                  ${PROJECT_SOURCE_DIR}/DRFPIC/include/*.hh
#		  )

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

#------------------
# dictionary
ROOT_GENERATE_DICTIONARY(ClassesDict GMCG4Classes.hh LINKDEF ${PROJECT_SOURCE_DIR}/GEOMSRVC/src/LinkDef.hh)
add_library(GMCG4ClassesDict SHARED
            ${PROJECT_SOURCE_DIR}/GEOMSRVC/src/GMCG4TrackerHit.cc
	    ${PROJECT_SOURCE_DIR}/GEOMSRVC/src/GMCG4Particle.cc
	    ${PROJECT_SOURCE_DIR}/GEOMSRVC/include/GMCG4Classes.hh
	    ClassesDict.cxx)

add_custom_command(TARGET GMCG4ClassesDict POST_BUILD
		   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/libClassesDict* ${PROJECT_BINARY_DIR}/lib
                   COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_BINARY_DIR}/libClassesDict*
		   )

#------------------
# add subdirectories
add_subdirectory(../ConfigReader ${CMAKE_CURRENT_BINARY_DIR}/ConfigReader)
add_subdirectory(EXCEP)
add_subdirectory(GEOMSRVC)
add_subdirectory(SVX)
add_subdirectory(PSHW)
add_subdirectory(CDCH)
add_subdirectory(TDCH)
add_subdirectory(PHCV)
add_subdirectory(DRFPIC)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

add_library(G4GMC SHARED ${sources})
target_include_directories(G4GMC
		PUBLIC ${PROJECT_SOURCE_DIR}/SVX/include
                PUBLIC ${PROJECT_SOURCE_DIR}/PSHW/include
                PUBLIC ${PROJECT_SOURCE_DIR}/CDCH/include
                PUBLIC ${PROJECT_SOURCE_DIR}/TDCH/include
                PUBLIC ${PROJECT_SOURCE_DIR}/PHCV/include
                PUBLIC ${PROJECT_SOURCE_DIR}/DRFPIC/include
		)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

add_executable(g4GMC g4GMC.cc)

target_link_libraries(g4GMC PUBLIC
		      G4GMC
		      GMCG4ClassesDict
		      SCrd
		      EXCEP
		      GEOMSRVC
		      SVX
		      PSHW
		      CDCH
		      TDCH
		      PHCV
		      DRFPIC
		      ${Geant4_LIBRARIES}
                      ${HEPMC_LIBRARIES}
		      ${HEPMC_FIO_LIBRARIES}
                      ${PYTHIA6_LIBRARIES}
		      ${ROOT_LIBRARIES}
#		      ${${PROJECT_NAME}_LIB}
#		      gfortran
#		      EDM4HEP::edm4hep
#		      podio::podioRootIO
		      )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B4a. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
#set(EXAMPLEB4A_SCRIPTS
#  exampleB4a.out
#  exampleB4.in
#  icons.mac
#  gui.mac
#  run.png
#  init_vis.mac
#  run1.mac
#  run2.mac
#  vis.mac
#  )

#foreach(_script ${EXAMPLEB4A_SCRIPTS})
#  configure_file(
#    ${PROJECT_SOURCE_DIR}/${_script}
#    ${PROJECT_BINARY_DIR}/${_script}
#    COPYONLY
#    )
#endforeach()

add_custom_command(TARGET g4GMC POST_BUILD
		   COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/config ${PROJECT_BINARY_DIR}/config
                   COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/g4mac ${PROJECT_BINARY_DIR}/g4mac			  )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS exampleB4a DESTINATION bin)
