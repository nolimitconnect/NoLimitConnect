
cmake_minimum_required (VERSION 3.22.1)

project(CoreLib, LANGUAGES CXX)

set(CoreLib_SOURCES)

list(APPEND CoreLib_SOURCES 
    AppErr_corelib.cpp
    AssetDefs.cpp
	BinaryBlob.cpp
    Blowfish_corelib.cpp
    DbBase_corelib.cpp
    DbDataset_corelib.cpp
    DbQueryData_corelib.cpp
    ObjectCommon_corelib.cpp
    OsDetect_corelib.cpp
    rsa_eay_corelib.c
    rsa_gen_corelib.c
    SHA1_corelib.cpp
    Sha1ClientInfo.cpp
    Sha1GeneratorMgr.cpp
    Sha1Info.cpp
    sqlite3_corelib.c
    sqlitedataset_corelib.cpp
    StdOutRedirect_corelib.cpp
	StringListBinary.cpp
    TimeIntervalEstimator.cpp
    VxAppInfo.cpp
    Invite.cpp
    VirtFile.cpp
    VxBuffer.cpp
    VxChop.cpp
    VxChopMap.cpp
    VxColorUtil.cpp
    VxCrypto.cpp
    VxDebug.cpp
    VxDefs.cpp
    VxFileCopier.cpp
    VxFileCrc32.cpp
    VxFileInfo.cpp
    VxFileIsTypeFunctions.cpp
    VxFileLists.cpp
    VxFileShredder.cpp
    VxFileShredderDb.cpp
    VxFileUtil.cpp
    VxFunctionsMissingInPosix.cpp
    VxFunctionsMissingInWindows.cpp
    VxGUID.cpp
    VxGUIDList.cpp
    VxGuidPairList.cpp
    VxGuidPairTimeList.cpp
    VxGlobals.cpp
    VxLinuxOnly.cpp
    VxMathDef.cpp
    VxMathVec3f.cpp
    VxMemUtils.cpp
    VxMutex.cpp
    VxParse.cpp
    VxProfile.cpp
    VxPtopUrl.cpp
    VxRefCount.cpp
    VxSemaphore.cpp
    VxSettings.cpp
    VxSha1Hash.cpp
    VxShortcut.cpp
    VxStringUtils.cpp
    VxTextStreamReader.cpp
    VxThread.cpp
    VxTimeUtil.cpp
    VxTime.cpp
    VxTimer.cpp
    VxValuePair.cpp
    VxXml.cpp
    VxUrl.cpp
    VxJava.cpp
    VxFunctionsMissingInAndroid.cpp
    VxMd5.cpp
)


add_library(CoreLib STATIC ${CoreLib_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET CoreLib
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
