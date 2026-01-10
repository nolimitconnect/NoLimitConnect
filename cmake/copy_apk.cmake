set(APK_DIR
    "${CMAKE_BINARY_DIR}/nolimitgui/android-build-nolimitconnect/build/outputs/apk/release"
)
set(DEPLOY_DIR "${CMAKE_SOURCE_DIR}/deploy/android")

include("${CMAKE_SOURCE_DIR}/cmake/version.cmake")

file(GLOB APK_FILES "${APK_DIR}/*.apk")

if(NOT APK_FILES)
    message(STATUS "No APK found, skipping copy")
    return()
endif()

list(GET APK_FILES 0 APK)

file(MAKE_DIRECTORY "${DEPLOY_DIR}")

set(OUT_APK "${DEPLOY_DIR}/nolimitconnect_v${NLC_VERSION}.apk")

message(STATUS "Copying APK:")
message(STATUS "  ${APK}")
message(STATUS "  -> ${OUT_APK}")

file(COPY_FILE "${APK}" "${OUT_APK}" ONLY_IF_DIFFERENT)
