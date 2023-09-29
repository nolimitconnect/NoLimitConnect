
cmake_minimum_required (VERSION 3.16.3)

project(xpm, LANGUAGES C CXX)

set(xpm_SOURCES)

list(APPEND xpm_SOURCES 
	src/Attrib.c
	src/CrBufFrI.c
	src/CrDatFrI.c
	src/CrIFrBuf.c
	src/CrIFrDat.c
	src/Image.c
	src/Info.c
	src/RdFToBuf.c
	src/RdFToDat.c
	src/RdFToI.c
	src/WrFFrBuf.c
	src/WrFFrDat.c
	src/WrFFrI.c
	src/create.c
	src/data.c
	src/hashtab.c
	src/misc.c
	src/parse.c
	src/rgb.c
	src/scan.c
	src/simx.c
)

add_library(xpm STATIC ${xpm_SOURCES})

target_include_directories(xpm
  PRIVATE
    include
    include/X11
)
