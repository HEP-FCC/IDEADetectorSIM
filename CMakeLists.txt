cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(IDEADetectorSIM)

set( CMAKE_CXX_STANDARD 17)
set( CMAKE_CXX_STANDARD_REQUIRED ON)

set( ROME_INST_DIR ${CMAKE_CURRENT_BINARY_DIR}/LOCAL/rome )
set( ENV{ROMESYS} ${ROME_INST_DIR} )
set( GMC_INST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/analyzer/GMC )
set( ENV{PRJBASE} ${CMAKE_CURRENT_SOURCE_DIR} )

message( STATUS "Install path ${CMAKE_INSTALL_PREFIX}" )
message( STATUS "Rome installation directory is ${ROME_INST_DIR}" )
message( STATUS "Romesys is ${ROMESYS}" )
message( STATUS "GMC_INST_DIR is ${GMC_INST_DIR}")

find_package(Geant4 REQUIRED)
find_package(podio REQUIRED)
find_package(EDM4HEP REQUIRED)


include(ExternalProject)

ExternalProject_Add(g4simulation
    PREFIX G4GMC
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/simulation/g4GMC
    BUILD_ALWAYS 1
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_FLAGS="-DMT_OFF"
)

ExternalProject_Add(rome3
    GIT_REPOSITORY https://bitbucket.org/muegamma/rome3.git	
    SOURCE_DIR ${ROME_INST_DIR}
    INSTALL_COMMAND ${CMAKE_COMMAND} -E create_symlink ${ROME_INST_DIR}/src/strlcpy.cxx ${ROME_INST_DIR}/src/strclpy.c
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E env ROMESYS=${ROME_INST_DIR} make 
    BUILD_IN_SOURCE TRUE
)

add_dependencies(rome3 g4simulation)

message( STATUS "G4INCLUDE will be set to ${Geant4_INCLUDE_DIR}" )

ExternalProject_Add(GMC
      SOURCE_DIR ${GMC_INST_DIR}
      CONFIGURE_COMMAND ""
      INSTALL_COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/analyzer/GMC/gmcanalyzer.exe ${CMAKE_CURRENT_BINARY_DIR}/bin/gmcanalyzer.exe
      BUILD_COMMAND
      ${CMAKE_COMMAND} -E env "ROMESYS=${ROME_INST_DIR}"
      ${CMAKE_COMMAND} -E env G4WORKDIR=${CMAKE_INSTALL_PREFIX}
      ${CMAKE_COMMAND} -E env GENFIT2SYS=$ENV{GENFIT}
      ${CMAKE_COMMAND} -E env EIGEN3SYS=$ENV{EIGEN3SYS}
      ${CMAKE_COMMAND} -E env ZLIB_DIR=$ENV{ZLIB_DIR}
      ${CMAKE_COMMAND} -E env G4INCLUDE=$ENV{G4INCLUDE}
      ${CMAKE_COMMAND} -E env G4LIBS=$ENV{G4LIBS}
      ${ROME_INST_DIR}/bin/romebuilder.exe -i GMC.xml
      BUILD_IN_SOURCE TRUE
)

add_dependencies(GMC rome3)

ExternalProject_Add(AnalysisTools
    PREFIX AnalysisTools
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/DRCalo/Analysis/AnalysisTools
    BUILD_ALWAYS 1
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_FLAGS="-DMT_OFF"
)

ExternalProject_Add(DRDigitization
    PREFIX DRDigitization
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/DRCalo/Digitization/DRDigitization
    BUILD_ALWAYS 1
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_FLAGS="-DMT_OFF"
)


ExternalProject_Add(converter
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/converter
    CONFIGURE_COMMAND
    ${CMAKE_COMMAND} -E env "ROMESYS=${ROME_INST_DIR}"
    ${CMAKE_COMMAND} -E env G4WORKDIR=${CMAKE_INSTALL_PREFIX}
    ${CMAKE_COMMAND} -E env GENFIT2SYS=$ENV{GENFIT}
    ${CMAKE_COMMAND} -E env EIGEN3SYS=$ENV{EIGEN3SYS}
    ${CMAKE_COMMAND} -E env ZLIB_DIR=$ENV{ZLIB_DIR}
    ${CMAKE_COMMAND} -E env G4INCLUDE=$ENV{G4INCLUDE}
    ${CMAKE_COMMAND} -E env G4LIBS=$ENV{G4LIBS}
    ${CMAKE_COMMAND} -E env SIM_INSTAL_DIR=${CMAKE_INSTALL_PREFIX}
    ${CMAKE_COMMAND} -E env PRJBASE=${CMAKE_SOURCE_DIR}
    ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_FLAGS="-DMT_OFF" ${CMAKE_SOURCE_DIR}/converter
    BUILD_ALWAYS 1
    BUILD_COMMAND ${MAKE} 
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_FLAGS="-DMT_OFF"
)

add_dependencies(converter GMC)
add_dependencies(converter DRDigitization)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/env.sh ${CMAKE_CURRENT_BINARY_DIR} @ONLY) 
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/xml_templates/geant4MC-IDEA-fit.xml ${CMAKE_CURRENT_BINARY_DIR} @ONLY) 
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/xml_templates/geant4MC-IDEA.xml ${CMAKE_CURRENT_BINARY_DIR} @ONLY) 
file(DOWNLOAD https://cernbox.cern.ch/index.php/s/KxGYRFnkcob09z1/download ${CMAKE_INSTALL_PREFIX}/g4-IDEA_reco.gdml )

install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/commonRecoIDEAFile.sh DESTINATION scripts )


