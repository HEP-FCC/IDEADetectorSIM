file(GLOB GEOMSRVC_SOURCES "src/*.cc")
include_directories(${PROJECT_SOURCE_DIR}/GEOMSRVC/include
		    ${PROJECT_SOURCE_DIR}/DRFPIC/include)
add_library(GEOMSRVC SHARED ${GEOMSRVC_SOURCES})