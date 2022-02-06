file(GLOB PHCV_SOURCES "src/*.cc")
include_directories(${PROJECT_SOURCE_DIR}/PHCV/include
		    ${PROJECT_SOURCE_DIR}/CDCH/include)
add_library(PHCV SHARED ${PHCV_SOURCES})
