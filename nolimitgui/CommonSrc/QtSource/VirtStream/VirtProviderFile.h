//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include <CoreLib/VFile.h>

#include <string>

#include <QFile>
#include <QString>

class VirtProviderFile : public QFile
{
public:
	VirtProviderFile() = default;
    VirtProviderFile( QString fileName );
	~VirtProviderFile() override;

    void                        closeFile( void );

	void						setError( int err )			{ m_Error = err; }
	int							getError( void )			{ return m_Error; }

	//=== vars ===//
	VFile*						m_VFile{ nullptr };
	std::string					m_FileName;
	std::string					m_FileMode;
	int							m_Error{ 0 };
};

