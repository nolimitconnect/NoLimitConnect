//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VirtProviderFile.h"

//============================================================================
VirtProviderFile::VirtProviderFile(QString fileName)
    : QFile( fileName )
{
}

//============================================================================
VirtProviderFile::~VirtProviderFile()
{
	closeFile();
}

//============================================================================
void VirtProviderFile::closeFile( void )
{
	close();
	delete m_VFile;
	m_VFile = nullptr;
    m_FileName.clear();
}

