cmake_minimum_required(VERSION 2.9)
project(qtdcs)

add_custom_target(qtdcs ALL
        COMMAND qmake ${CMAKE_CURRENT_SOURCE_DIR}/qt/qtdcs.pro 
		  COMMAND make -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
		  COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/qtdcs ${CMAKE_CURRENT_BINARY_DIR}/../bin/.
)


message(INFO ${Green} " Source file=" ${CMAKE_CURRENT_SOURCE_DIR}  ${White}) 
