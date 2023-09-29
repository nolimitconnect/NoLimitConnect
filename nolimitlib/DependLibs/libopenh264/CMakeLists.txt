
cmake_minimum_required (VERSION 3.16.3)

project(openh264, LANGUAGES C CXX)

set(openh264_SOURCES)

list(APPEND openh264_SOURCES 
)

add_library(openh264 STATIC ${openh264_SOURCES})

target_include_directories(openh264
  PRIVATE

)
