cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(DriftChamberPLUSVertex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message( STATUS "Install path ${CMAKE_INSTALL_PREFIX}" )

add_subdirectory(simulation/g4GMC)
