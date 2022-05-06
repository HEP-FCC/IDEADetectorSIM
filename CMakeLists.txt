cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(DriftChamberPLUSVertex)

set( CMAKE_CXX_STANDARD 17)
set( CMAKE_CXX_STANDARD_REQUIRED ON)

set( ROME_INST_DIR ${CMAKE_CURRENT_BINARY_DIR}/LOCAL/rome )
set( ROMESYS ${ROME_INST_DIR} )
set( GMC_INST_DIR ${CMAKE_SOURCE_DIR}/analyzer/GMC )


message( STATUS "Install path ${CMAKE_INSTALL_PREFIX}" )
message( STATUS "Rome installation directory is ${ROME_INST_DIR}" )
message( STATUS "Romesys is ${ROMESYS}" )
message( STATUS "GMC_INST_DIR is ${GMC_INST_DIR}")


add_subdirectory(simulation/g4GMC)


include(ExternalProject)


ExternalProject_Add(rome3
    GIT_REPOSITORY https://bitbucket.org/muegamma/rome3.git	
    SOURCE_DIR ${ROME_INST_DIR}
    INSTALL_COMMAND ${CMAKE_COMMAND} -E create_symlink ${ROME_INST_DIR}/src/strlcpy.cxx  ${ROME_INST_DIR}/src/strclpy.c 
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E env ROMESYS=${ROME_INST_DIR} make 
    BUILD_IN_SOURCE TRUE
)

ExternalProject_Add(GMC
      SOURCE_DIR ${GMC_INST_DIR}
      INSTALL_COMMAND ${CMAKE_COMMAND} -E create_symlink ${ROME_INST_DIR}/src/strlcpy.cxx  ${ROME_INST_DIR}/src/strclpy.c 
      CONFIGURE_COMMAND ""
      BUILD_COMMAND
      ${CMAKE_COMMAND} -E env "ROMESYS=${ROME_INST_DIR}"
      ${CMAKE_COMMAND} -E env G4WORKDIR=${CMAKE_INSTALL_PREFIX}
      ${CMAKE_COMMAND} -E env GENFIT2SYS=$ENV{GENFIT}
      ${CMAKE_COMMAND} -E env EIGEN3SYS=$ENV{EIGEN3SYS}
      ${CMAKE_COMMAND} -E env ZLIB_DIR=$ENV{ZLIB_DIR}	
      ${ROME_INST_DIR}/bin/romebuilder.exe -i GMC.xml
      BUILD_IN_SOURCE TRUE
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/env.sh ${CMAKE_CURRENT_BINARY_DIR} @ONLY) 
