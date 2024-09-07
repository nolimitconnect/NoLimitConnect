#pragma once
//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include <string>
#include <CoreLib/VxFileTypeMasks.h>

#include <QString>

// Android 11 (API level 30) implemented scoped storage which is why this class exists

class VxFileInfoBase;
class VxGUID;
class QFileInfo;
class QUrl;

class VirtStorageProvider
{

public:
    VirtStorageProvider() = default;
    VirtStorageProvider( const VirtStorageProvider& rhs ) = delete;
	virtual ~VirtStorageProvider() = default;


    void				fromGuiBrowseFiles( VxGUID& appInstId, std::string& folderName, uint8_t fileFilterMask = VXFILE_TYPE_ALLNOTEXE | VXFILE_TYPE_DIRECTORY );

protected:
    bool                requestAndroidStoragePermissions( void );

    void                loadUrl( const QUrl &url );

    bool                requestPermission( QString permissionName ); // returns false if user denies permission to use android hardware

};

VirtStorageProvider& GetVirtStorageProvider( void );


