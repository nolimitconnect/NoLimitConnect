if(NOT DEFINED APK_FILE_PRIMARY)
    set(APK_FILE_PRIMARY "")
endif()
if(NOT DEFINED APK_OUTPUT_DIR_RELEASE)
    message(FATAL_ERROR "APK_OUTPUT_DIR_RELEASE is not defined")
endif()
if(NOT DEFINED APK_OUTPUT_DIR_DEBUG)
    message(FATAL_ERROR "APK_OUTPUT_DIR_DEBUG is not defined")
endif()
if(NOT DEFINED PACKAGE_OUTPUT_DIR)
    message(FATAL_ERROR "PACKAGE_OUTPUT_DIR is not defined")
endif()
if(NOT DEFINED PACKAGE_FILENAME)
    message(FATAL_ERROR "PACKAGE_FILENAME is not defined")
endif()

set(_apk_to_copy "")
set(_apk_candidates)

if(APK_FILE_PRIMARY AND EXISTS "${APK_FILE_PRIMARY}")
    list(APPEND _apk_candidates "${APK_FILE_PRIMARY}")
endif()

set(_apk_candidate_dirs
    "${APK_OUTPUT_DIR_RELEASE}"
    "${APK_OUTPUT_DIR_DEBUG}"
)

foreach(_apk_dir IN LISTS _apk_candidate_dirs)
    if(EXISTS "${_apk_dir}")
        file(GLOB _apk_files "${_apk_dir}/*.apk")
        if(_apk_files)
            list(APPEND _apk_candidates ${_apk_files})
        endif()
    endif()
endforeach()

if(_apk_candidates)
    list(REMOVE_DUPLICATES _apk_candidates)

    foreach(_apk_candidate IN LISTS _apk_candidates)
        get_filename_component(_apk_name "${_apk_candidate}" NAME)
        if(NOT _apk_name MATCHES "(unsigned|unaligned)")
            set(_apk_to_copy "${_apk_candidate}")
            break()
        endif()
    endforeach()
endif()

if(_apk_to_copy STREQUAL "" AND _apk_candidates)
    list(GET _apk_candidates 0 _apk_to_copy)
endif()

if(_apk_to_copy STREQUAL "")
    message(STATUS "No APK found in expected Android output paths. Skipping package copy.")
    return()
endif()

file(MAKE_DIRECTORY "${PACKAGE_OUTPUT_DIR}")
set(_out_apk "${PACKAGE_OUTPUT_DIR}/${PACKAGE_FILENAME}")

message(STATUS "Copying Android package:")
message(STATUS "  ${_apk_to_copy}")
message(STATUS "  -> ${_out_apk}")

file(COPY_FILE "${_apk_to_copy}" "${_out_apk}" ONLY_IF_DIFFERENT)
