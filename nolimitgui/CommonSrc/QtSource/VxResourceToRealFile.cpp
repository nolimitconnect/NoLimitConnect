//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxResourceToRealFile.h"

#include <CoreLib/VxGlobals.h>

#include <QFileInfo>
#include <QDir>
#include <QDebug>

//============================================================================
VxResourceToRealFile::VxResourceToRealFile( const QString & resPath )
: QFile( nullptr )
{
    setObjectName( "VxResourceToRealFile" );
	QFileInfo resPathInfo( resPath );
	std::string appResDirStr = VxGetAppNoLimitDataDirectory() + "appres/";
	QString appResDirQString( appResDirStr.c_str() );
	QDir appResDir;
	if( !appResDir.cd( appResDirQString ) )
	{
		appResDir.mkdir( appResDirQString );
		appResDir.cd( appResDirQString );
	}

	QString onDiskFileName = appResDir.filePath( resPathInfo.fileName() );
	setFileName( onDiskFileName );
	if( !exists() )
	{
		QFile resfile( resPath );
		if( resfile.open( QIODevice::ReadOnly ) )
		{
			if( open( QIODevice::ReadWrite ) )
			{
				if( 0 == write( resfile.readAll() ) )
				{
					qWarning() << "Could not write app resource file " << onDiskFileName;
				}
				else
				{
					m_FilePathAndName = onDiskFileName.toUtf8().constData();
				}

				close();
			}
			else
			{
				qWarning() << "Could not open app resource file for writing " << onDiskFileName;
			}

			resfile.close();
		}
		else
		{
			qWarning() << "Could not open resource file " << resPathInfo.fileName();
		}
	}
	else
	{
		m_FilePathAndName = onDiskFileName.toUtf8().constData();
	}
}