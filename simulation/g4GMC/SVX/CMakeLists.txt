file(GLOB SVX_SOURCES "src/*.cc")
include_directories(${PROJECT_SOURCE_DIR}/SVX/include
		    ${PROJECT_SOURCE_DIR}/CDCH/include)
add_library(SVX SHARED ${SVX_SOURCES})

