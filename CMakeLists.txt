cmake_minimum_required(VERSION 2.9)
project(qtdcs)


IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  FIND_PACKAGE(Git)
  IF(GIT_FOUND)
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "GIT_BUILD_VERSION"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    MESSAGE( STATUS "Git version: ${BUILD_VERSION}" )
  ELSE(GIT_FOUND)
    SET(GIT_BUILD_VERSION 0)
  ENDIF(GIT_FOUND)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)

message(INFO ${Green} " git version=" ${GIT_BUILD_VERSION}  ${White}) 

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/qt/Version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/qt/Version.h )


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Xml REQUIRED)

include_directories(${Qt5Widgets_INCLUDES} ./qt/ ../include  ${LIB_SHM_PATH}/include /usr/local/qwt/include  ${LIB_DECODEFRAME_PATH}/include /usr/local/include)

add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUI ON)


set(QWT_LIBRARY_NAMES qwt-qt5 qwt6-qt5 qwt qwt6)
find_library(QWT_LIBRARIES
  NAMES ${QWT_LIBRARY_NAMES}
  PATHS
    /usr/lib
    /usr/local/qwt/lib
    /usr/local/lib/qt5
    "$ENV{LIB_DIR}/lib"
    "$ENV{LIB}"
)

set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

file (
	GLOB_RECURSE

	source_files
	qt/*.c
	qt/*.cpp
        
	qt/*.h     
	../include/*.h
)


add_executable(
	qtdcs 
	${source_files}
	${UIS_HDRS}
)

target_link_libraries(qtdcs ${Qt5Widgets_LIBRARIES} Qt5::PrintSupport Qt5::Xml ringshm ${QWT_LIBRARIES} frame)




