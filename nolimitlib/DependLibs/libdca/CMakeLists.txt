
cmake_minimum_required (VERSION 3.16.3)

project(dca, LANGUAGES C CXX)

set(dca_SOURCES)

list(APPEND dca_SOURCES 
	bitstream.c
	downmix.c
	parse.c
)

add_library(dca STATIC ${dca_SOURCES})
