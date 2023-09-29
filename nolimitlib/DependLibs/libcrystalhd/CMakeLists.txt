
cmake_minimum_required (VERSION 3.16.3)

project(crystalhd LANGUAGES C CXX)

set(crystalhd_SOURCES)

list(APPEND crystalhd_SOURCES 
	src/libcrystalhd_fwcmds.c
	src/libcrystalhd_fwdiag_if.c
	src/libcrystalhd_fwload_if.c
	src/libcrystalhd_if.c
	src/libcrystalhd_int_if.c
	src/libcrystalhd_parser.c
	src/libcrystalhd_priv.c
)

add_library(crystalhd STATIC ${crystalhd_SOURCES})
