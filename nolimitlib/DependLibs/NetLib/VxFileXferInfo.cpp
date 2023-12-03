//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxFileXferInfo.h"

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxFileUtil.h>

#include <stdio.h>

//============================================================================
VxFileXferInfo::VxFileXferInfo( VxGUID& lclSessionId )
: m_LclSessionId( lclSessionId )
{
}

//============================================================================
VxFileXferInfo::~VxFileXferInfo()
{
	if( m_hFile )
	{
		fclose(m_hFile);
        m_hFile = nullptr;
	}
}

//============================================================================
bool VxFileXferInfo::calcProgress( void ) 
{ 
	int prevProgress = m_PercentProgress;
	if( 0 != m_u64FileLen ) 
	{
		m_PercentProgress = (int)((m_u64FileOffs * 100) / m_u64FileLen);
	}

	return ( m_PercentProgress != prevProgress ); 
}

//============================================================================
std::string VxFileXferInfo::getDownloadIncompleteFileName( void )
{
	return m_strLocalFileName;
}

//============================================================================
std::string VxFileXferInfo::getDownloadCompleteFileName( void )
{
    std::string completedFileName = eAssetTypeThumbnail == getAssetType() ? VxGetAppThumbnailDirectory() : VxGetDownloadsDirectory();
	std::string strPath;
	std::string strAssetNameOnly;
	RCODE rc = VxFileUtil::seperatePathAndFile(	m_strLocalFileName,			
												strPath,			
												strAssetNameOnly );	
	if( 0 == rc )
	{
		completedFileName += strAssetNameOnly;
	}

	return completedFileName;
}
