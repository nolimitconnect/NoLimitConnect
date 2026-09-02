//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

// manage the ever changing android permissions
// NOTE: even if you have broad permissions like  "android.permission.MANAGE_EXTERNAL_STORAGE"
// does not mean you have narrow permissions like "android.permission.READ_MEDIA_VIDEO"

#include "GuiHelpers.h"
#include "GuiParams.h"

#include <CoreLib/AssetDefs.h>
#include <CoreLib/VxDebug.h>

#include <QFileSystemModel>
#include <QStandardPaths>
#include <QUrl>

#if defined (Q_OS_ANDROID)
# if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#  include <QtAndroid>
# else
#  include <QtCore/private/qandroidextras_p.h>
#  include <QJniObject>
# endif
#endif //defined (Q_OS_ANDROID)

//============================================================================
int GuiHelpers::getAndroidSDKVersion( void )
{
#if defined (Q_OS_ANDROID)
    return QJniObject::getStaticField<jint>(
        "android/os/Build$VERSION", "SDK_INT");
#endif // defined (Q_OS_ANDROID)

    return 0;
}

//============================================================================
bool GuiHelpers::havePermission( QString permissionName ) // returns false if user denies permission to use android hardware
{
#if defined (Q_OS_ANDROID)
    QtAndroidPrivate::PermissionResult result = QtAndroidPrivate::checkPermission( permissionName ).result();
    if( QtAndroidPrivate::Authorized != result )
    {
        if( QtAndroidPrivate::Denied == result )
        {
            return false;
        }
    }
#endif // defined (Q_OS_ANDROID)

    return true;
}

//============================================================================
bool GuiHelpers::requestFilePermission( enum EMediaFileType permissionType, bool showUserMsgIfDenied )
{
#ifdef Q_OS_ANDROID
    if( havePermission(QLatin1String("android.permission.MANAGE_EXTERNAL_STORAGE")) )
    {
        return true;
    }

    if( havePermission(QLatin1String("android.permission.WRITE_EXTERNAL_STORAGE")) )
    {
        return true;
    }

    if( havePermission(QLatin1String("android.permission.READ_EXTERNAL_STORAGE")) )
    {
        return true;
    }

    bool result{true};

    QStringList permissionList;
    if( getAndroidSDKVersion() >= 33 )
    {
        switch(permissionType)
        {
        case eMediaFileAny:
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_VIDEO"));
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_AUDIO"));
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_IMAGES"));
            break;

        case eMediaFileImage:
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_IMAGES"));
            break;

        case eMediaFileAudio:
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_AUDIO"));
            break;

        case eMediaFileVideo:
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_VIDEO"));
            break;

        default:
            break;
        }
    }
    else
    {
        permissionList.emplace_back(QLatin1String("android.permission.READ_EXTERNAL_STORAGE"));
    }

    for( auto permission : permissionList )
    {
        if( !GuiParams::requestPermission( permission ) )
        {
            LogMsg( LOG_ERROR, "%s permission denied %s", __func__, permission.toUtf8().constData() );
            result = false;
            break;
        }
    }

    if(!result && showUserMsgIfDenied)
    {
        showFilePermissionError();
    }

    return result;
#else
    return true;
#endif // Q_OS_ANDROID
}
