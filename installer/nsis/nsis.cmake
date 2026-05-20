

# Path to already-built EXE
set(APP_EXE "${PROJECT_SRC_ROOT_DIR}/nolimitgui/bin-Windows/NoLimitConnect.exe")

if(NOT EXISTS "${APP_EXE}")
  message(FATAL_ERROR "NoLimitConnect.exe not found: ${APP_EXE}")
endif()

set(PACKAGE_DIR "${DEPLOY_DIR}/windows")
file(MAKE_DIRECTORY "${PACKAGE_DIR}")

# Find tools
find_program(MAKENSIS_EXECUTABLE makensis REQUIRED)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt REQUIRED)

find_program(SIGNTOOL_EXECUTABLE
    NAMES signtool
    HINTS
        "C:/Program Files (x86)/Windows Kits/10/bin/x64"
        "C:/Program Files (x86)/Windows Kits/11/bin/x64"
    REQUIRED
)

# Staging directory
set(STAGE_DIR "${CMAKE_BINARY_DIR}/stage")

add_custom_target(stage ALL
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${STAGE_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGE_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "${APP_EXE}" "${STAGE_DIR}"
)

# Deploy Qt runtime
add_custom_target(deployqt ALL
  COMMAND "${WINDEPLOYQT_EXECUTABLE}"
          --release
          --no-compiler-runtime
          --no-translations
          "${STAGE_DIR}/NoLimitConnect.exe"
  WORKING_DIRECTORY "${STAGE_DIR}"
)

add_dependencies(deployqt stage)

set(INSTALLER_FILENAME
    "${NLC_NAME}-${PROJECT_VERSION}-windows.exe"
)

# sign the application executable
# add_custom_target(sign_app ALL
#     COMMAND "${SIGNTOOL_EXECUTABLE}" sign
#         /fd SHA256
#         /tr http://timestamp.digicert.com
#         /td SHA256
#         "$<SHELL_PATH:${STAGE_DIR}/NoLimitConnect.exe>"
#     DEPENDS deployqt
#     COMMENT "Signing application executable"
# )

# hash application executable
set(HASH_DIR "${PACKAGE_DIR}")

set(APP_HASH_FILE
    "${HASH_DIR}/NoLimitConnect.exe.sha256"
)

set(INSTALLER_HASH_FILE
    "${HASH_DIR}/${INSTALLER_FILENAME}.sha256"
)

add_custom_target(hash_app ALL
    COMMAND ${CMAKE_COMMAND} -E sha256sum
        "${STAGE_DIR}/NoLimitConnect.exe"
        > "${APP_HASH_FILE}"
    DEPENDS deployqt
    COMMENT "Generating SHA256 for application executable"
)

# create installer

set(CPACK_RESOURCE_FILE_LICENSE
    "${CMAKE_SOURCE_DIR}/LICENSE.GPL"
)

set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME ${NLC_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_NSIS_DISPLAY_NAME "${NLC_NAME} ${PROJECT_VERSION}")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_PACKAGE_FILE_NAME "${INSTALLER_FILENAME}")

set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}")

if(POLICY CMP0177)
  cmake_policy(SET CMP0177 NEW)
endif()

# Build nsis package
install(DIRECTORY
    "${STAGE_DIR}/"
    DESTINATION .
)

include(CPack)

add_custom_target(package_nsis ALL
    COMMAND ${CMAKE_COMMAND} --build . --target deployqt
    COMMAND cpack -G NSIS --config CPackConfig.cmake

    # sign the nisis package
    # COMMAND "${SIGNTOOL_EXECUTABLE}" sign
    #     /fd SHA256
    #     /tr http://timestamp.digicert.com
    #     /td SHA256
    #     "${CMAKE_BINARY_DIR}/${INSTALLER_FILENAME}"

    file(REMOVE "${PACKAGE_DIR}/${INSTALLER_FILENAME}")
    file(REMOVE "${PACKAGE_DIR}/${INSTALLER_HASH_FILE}")

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/${INSTALLER_FILENAME}"
        "${PACKAGE_DIR}/${INSTALLER_FILENAME}"

    # create hash of the nisis package
    COMMAND ${CMAKE_COMMAND} -E sha256sum
        "${PACKAGE_DIR}/${INSTALLER_FILENAME}"
        > "${INSTALLER_HASH_FILE}"

    # DEPENDS deployqt
    # DEPENDS sign_app
    DEPENDS hash_app
    COMMENT "Building NSIS installer and copying to ${PACKAGE_DIR}/"
)

