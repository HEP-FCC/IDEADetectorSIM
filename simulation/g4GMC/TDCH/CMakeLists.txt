file(GLOB TDCH_SOURCES "src/*.cc")
include_directories(${PROJECT_SOURCE_DIR}/TDCH/include
		    ${PROJECT_SOURCE_DIR}/CDCH/include)
add_library(TDCH SHARED ${TDCH_SOURCES})
