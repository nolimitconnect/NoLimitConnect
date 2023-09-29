
cmake_minimum_required (VERSION 3.16.3)

project(kvazaar, LANGUAGES C CXX)

set(kvazaar_SOURCES)

list(APPEND kvazaar_SOURCES 
	src/bitstream.c
	src/cabac.c
	src/cfg.c
	src/context.c
	src/cu.c
	src/encode_coding_tree.c
	src/encoder.c
	src/encoder_state-bitstream.c
	src/encoder_state-ctors_dtors.c
	src/encoder_state-geometry.c
	src/encoderstate.c
	src/extras/crypto.cpp
	src/extras/getopt.c
	src/extras/libmd5.c
	src/filter.c
	src/image.c
	src/imagelist.c
	src/input_frame_buffer.c
	src/inter.c
	src/intra.c
	src/kvazaar.c
	src/nal.c
	src/rate_control.c
	src/rdo.c
	src/sao.c
	src/scalinglist.c
	src/search.c
	src/search_inter.c
	src/search_intra.c
	src/strategies/altivec/picture-altivec.c
	src/strategies/avx2/dct-avx2.c
	src/strategies/avx2/intra-avx2.c
	src/strategies/avx2/ipol-avx2.c
	src/strategies/avx2/picture-avx2.c
	src/strategies/avx2/quant-avx2.c
	src/strategies/avx2/sao-avx2.c
	src/strategies/generic/dct-generic.c
	src/strategies/generic/intra-generic.c
	src/strategies/generic/ipol-generic.c
	src/strategies/generic/nal-generic.c
	src/strategies/generic/picture-generic.c
	src/strategies/generic/quant-generic.c
	src/strategies/generic/sao-generic.c
	src/strategies/sse2/picture-sse2.c
	src/strategies/sse41/picture-sse41.c
	src/strategies/strategies-dct.c
	src/strategies/strategies-intra.c
	src/strategies/strategies-ipol.c
	src/strategies/strategies-nal.c
	src/strategies/strategies-picture.c
	src/strategies/strategies-quant.c
	src/strategies/strategies-sao.c
	src/strategies/x86_asm/picture-x86-asm.c
	src/strategyselector.c
	src/tables.c
	src/threadqueue.c
	src/transform.c
	src/videoframe.c
)

add_library(kvazaar STATIC ${kvazaar_SOURCES})
