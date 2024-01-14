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

class VirtStreamFile
{
public:
	VirtStreamFile() = delete;
	VirtStreamFile( VFile* vFile, const char* fileName, const char* fileMode );


protected:
	//=== vars ===//
	VFile*						m_VFile{ nullptr };
	std::string					m_FileName;
	std::string					m_FileMode;
};
