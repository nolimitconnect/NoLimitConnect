//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_ANDROID)

#include "VirtProviderFile.h"

//============================================================================
VirtProviderFile::VirtProviderFile(QString fileName)
    : QFile( fileName )
{
    closeFile();
}

//============================================================================
VirtProviderFile::~VirtProviderFile()
{
	closeFile();
}

//============================================================================
void VirtProviderFile::clear( void )
{
	setError( 0 );
	m_FileName.clear();
	m_FileMode.clear();
}

//============================================================================
void VirtProviderFile::closeFile( void )
{
	close();
	delete m_VFile;
	m_VFile = nullptr;
	clear();
}

#endif // defined(TARGET_OS_ANDROID)
