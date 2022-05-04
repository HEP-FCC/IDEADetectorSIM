cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(DriftChamberPLUSVertex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROMESYS "$ENV{ROMESYS}")

message( STATUS "Install path ${CMAKE_INSTALL_PREFIX}" )

#add_subdirectory(simulation/g4GMC)


include(ExternalProject)

ExternalProject_Add(rome3
    GIT_REPOSITORY https://bitbucket.org/muegamma/rome3.git	
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/rome3    
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/rome3
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E env ROMESYS=${SOURCE_DIR} make 
    BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND make DESTDIR=${CMAKE_INSTALL_PREFIX} install
)
