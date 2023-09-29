
cmake_minimum_required (VERSION 3.16.3)

project(shairplay, LANGUAGES C CXX)

set(shairplay_SOURCES)

list(APPEND shairplay_SOURCES 
	alac/alac.c
	base64.c
	crypto/aes.c
	crypto/bigint.c
	crypto/hmac.c
	crypto/md5.c
	crypto/rc4.c
	crypto/sha1.c
	digest.c
	dnssd.c
	http_parser.c
	http_request.c
	http_response.c
	httpd.c
	logger.c
	netutils.c
	raop.c
	raop_buffer.c
	raop_rtp.c
	rsakey.c
	rsapem.c
	sdp.c
	shairplay.c
	utils.c
)

add_library(shairplay STATIC ${shairplay_SOURCES})
