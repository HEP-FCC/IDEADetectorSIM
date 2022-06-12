# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)


# Create a project:
project( DRCalo_AnalysisTools )

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

file(GLOB AnalysisTools_h "include/*.h")
file(GLOB AnalysisTools_cxx "src/*.cxx")
file(GLOB AnalysisTools_dict_cxx "G__*.cxx")

message(STATUS ${AnalysisTools_cxx})

add_library( DRCalo_AnalysisToolsLib SHARED ${AnalysisTools_h} ${AnalysisTools_cxx})

target_link_libraries(DRCalo_AnalysisToolsLib ${ROOT_LIBRARIES})
target_include_directories( DRCalo_AnalysisToolsLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )	  

install(FILES ${AnalysisTools_h} DESTINATION include/AnalysisTools)
install( TARGETS DRCalo_AnalysisToolsLib DESTINATION lib )




