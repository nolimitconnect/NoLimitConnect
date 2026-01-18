cmake_minimum_required(VERSION 3.21)

set(CPU_ARCH "_amd64")

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "ARM")
    message(STATUS "Copy ARM flatpack")
    set(CPU_ARCH "_arm64")
else()
    message(STATUS "Copy amd64 flatpack")
    set(CPU_ARCH "_amd64")
endif()

set(IN_FLATPACK "${PROJECT_SRC_ROOT_DIR}/repo")

set(PACKAGE_DIR "${DEPLOY_DIR}/linux")
file(MAKE_DIRECTORY "${PACKAGE_DIR}")

set(OUT_FLATPACK "${PACKAGE_DIR}/nolimitconnect_v${NLC_VERSION}.flatpack")

message(STATUS "Copying FLATPACK:")
message(STATUS "  ${IN_FLATPACK}")
message(STATUS "  -> ${OUT_FLATPACK}")

file(COPY_FILE "${IN_FLATPACK}" "${OUT_FLATPACK}" ONLY_IF_DIFFERENT)


