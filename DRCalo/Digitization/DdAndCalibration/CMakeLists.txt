# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Create a project:
project( DRCalo_DdAndCalibration )

find_package(EDM4HEP)
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

file(GLOB DdAndCalibration_h "include/*.h")
file(GLOB DdAndCalibration_cxx "src/*.cxx")

add_library( DRCalo_DdAndCalibrationLib SHARED ${DdAndCalibration_h} ${DdAndCalibration_cxx})
target_link_libraries(DRCalo_DdAndCalibrationLib ${ROOT_LIBRARIES} EDM4HEP::edm4hep)
target_include_directories(DRCalo_DdAndCalibrationLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include )

add_executable( TestDdAndCalibration ${CMAKE_CURRENT_SOURCE_DIR}/utils/TestCalibration.cxx )
add_executable( TestGridHelper ${CMAKE_CURRENT_SOURCE_DIR}/utils/TestGridHelper.cxx )                                                             
target_link_libraries( TestDdAndCalibration DRCalo_DdAndCalibrationLib )
target_link_libraries( TestGridHelper DRCalo_DdAndCalibrationLib ) 

install( TARGETS TestDdAndCalibration DESTINATION bin )
install( TARGETS TestGridHelper DESTINATION bin )      
install(FILES ${DdAndCalibration_h} DESTINATION include/DdAndCalibration)
install( TARGETS DRCalo_DdAndCalibrationLib DESTINATION lib )




