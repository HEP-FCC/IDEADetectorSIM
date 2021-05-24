# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(g4GMC_simulation)

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

#----------------------------------------------------------------------------
# Find ROOT
#
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO EG)
include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# THIS PROJECT
#--------------------------------------------------------------------
# headers
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR}
                    ${HEPMC_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}/../ConfigReader/include
		    )
foreach(basefld ${BASE_FLD})
include_directories(${PROJECT_SOURCE_DIR}/${basefld}/include)
endforeach()

# -------------------------------------------------------------------
# source
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)

# -------------------------------------------------------------------
# output library
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# -------------------------------------------------------------------
# output binaries
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#--------------------------------------------------------------------
# dictionary
ROOT_GENERATE_DICTIONARY(ClassesDict GMCG4Classes.hh LINKDEF ${PROJECT_SOURCE_DIR}/GEOMSRVC/src/LinkDef.hh)
add_library(GMCG4ClassesDict SHARED
            ${PROJECT_SOURCE_DIR}/GEOMSRVC/src/GMCG4TrackerHit.cc
	    ${PROJECT_SOURCE_DIR}/GEOMSRVC/src/GMCG4Particle.cc
	    ${PROJECT_SOURCE_DIR}/GEOMSRVC/include/GMCG4Classes.hh
	    ClassesDict.cxx
	    )

add_custom_command(TARGET GMCG4ClassesDict POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/libClassesDict* ${PROJECT_BINARY_DIR}/lib
            	    	  COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_BINARY_DIR}/libClassesDict*
		    	  )

#--------------------------------------------------------------------
# add subdirectories

add_subdirectory(../ConfigReader ${CMAKE_CURRENT_BINARY_DIR}/ConfigReader)
foreach(basefld ${BASE_FLD})
	add_subdirectory(${basefld})
endforeach()
foreach(detfld ${DET_FLD})
	add_subdirectory(${detfld})
endforeach()

#--------------------------------------------------------------------
# Make progect shared libraries
#

add_library(G4GMC SHARED ${sources})
foreach(detfld ${DET_FLD})
	target_include_directories(G4GMC
	PUBLIC ${PROJECT_SOURCE_DIR}/${detfld}/include
	)
endforeach()

#--------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

add_executable(g4GMC g4GMC.cc)

target_link_libraries(g4GMC PUBLIC
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
		      EDM4HEP::edm4hep
		      podio::podioRootIO
		      )

#--------------------------------------------------------------------
# Link configure files

add_custom_command(TARGET g4GMC POST_BUILD
		   COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/config ${PROJECT_BINARY_DIR}/config
		   COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/g4mac ${PROJECT_BINARY_DIR}/g4mac
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
#--------------------------------------------------------------------
# convertTracks.cc
#
#add_executable(convertTracks convertTracks.cc)

#target_link_libraries(convertTracks PUBLIC
#                      G4GMC
#                      GMCG4ClassesDict
#                      SCrd
#                      ${BASE_FLD}
#                      ${DET_FLD}
#                      ${Geant4_LIBRARIES}
#                      ${HEPMC_LIBRARIES}
#                      ${HEPMC_FIO_LIBRARIES}
#                      ${PYTHIA6_LIBRARIES}
#                      ${ROOT_LIBRARIES}
#                      ${${PROJECT_NAME}_LIB}
#                      gfortran
#                     EDM4HEP::edm4hep
#                     podio::podioRootIO
#                     )

#--------------------------------------------------------------------
# Copy local headers for this project
#
# file(COPY
# ${PROJECT_SOURCE_DIR}/../ConfigReader/include/
# DESTINATION include/ConfigReader
# )
add_custom_command(
#	linkConfigReader ALL
	TARGET g4GMC POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/include
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/../ConfigReader/include ${PROJECT_BINARY_DIR}/include/ConfigReader
)
foreach(basefld ${BASE_FLD})
# file(COPY
# ${PROJECT_SOURCE_DIR}/${basefld}/include/
# DESTINATION include/${basefld}
# )
add_custom_command(
	# OUTPUT link${basefld}
	TARGET g4GMC POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/${basefld}/include ${PROJECT_BINARY_DIR}/include/${basefld}
	)
endforeach()
foreach(detfld ${DET_FLD})
	# file(COPY
# ${PROJECT_SOURCE_DIR}/${detfld}/include/
	# DESTINATION include/${detfld}
  	# )
add_custom_command(
	TARGET g4GMC POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/${detfld}/include ${PROJECT_BINARY_DIR}/include/${detfld}
	)
endforeach()

#--------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS g4GMC DESTINATION bin)

install(DIRECTORY ${PROJECT_BINARY_DIR}/lib DESTINATION .)

install(DIRECTORY ../ConfigReader/include/ DESTINATION include/ConfigReader)
foreach(basefld ${BASE_FLD})
	install(DIRECTORY ${basefld}/include/ DESTINATION include/${basefld})
endforeach()
foreach(detfld ${DET_FLD})
	install(DIRECTORY ${detfld}/include/ DESTINATION include/${detfld})
endforeach()

install(DIRECTORY g4mac DESTINATION .)
install(DIRECTORY config DESTINATION .)
