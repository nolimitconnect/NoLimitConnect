#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QFile>
#include <QString>
#include <string>

// copy resource from qt resources to a real file in app_root_data_dir/appres/
class VxResourceToRealFile : public QFile
{
public:
	VxResourceToRealFile( const QString& resPath );

	std::string& getRealFilePathAndName( void ) { return m_FilePathAndName; }

	std::string m_FilePathAndName;
};
