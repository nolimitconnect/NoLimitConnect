//============================================================================
// Copyright (C) 2015 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <config_appcorelibs.h>
#include "BlobInfo.h"

#include <PktLib/VxSearchDefs.h>

#include <CoreLib/VxFileLists.h>
#include <CoreLib/VxFileIsTypeFunctions.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <sys/types.h>
#include <sys/stat.h>

//============================================================================
BlobInfo::BlobInfo()
: AssetBaseInfo()
{ 
}

//============================================================================
BlobInfo::BlobInfo( const BlobInfo& rhs )
{
	*((AssetBaseInfo*)this) = rhs;
}

//============================================================================
BlobInfo::BlobInfo( const AssetBaseInfo& rhs )
{
    *((AssetBaseInfo*)this) = rhs;
}

//============================================================================
BlobInfo::BlobInfo( EAssetType assetType, const std::string& fileName )
: AssetBaseInfo( assetType, fileName )
{ 
}

//============================================================================
BlobInfo::BlobInfo( EAssetType assetType, const std::string& fileName, VxGUID& assetId )
	: AssetBaseInfo( assetType, fileName, assetId )
{
}

//============================================================================
BlobInfo::BlobInfo( EAssetType assetType, const char* fileName, uint64_t fileLen )
: AssetBaseInfo( assetType, fileName, fileLen )
{
}

//============================================================================
BlobInfo::BlobInfo( EAssetType assetType, const char* fileName, uint64_t fileLen, VxGUID& assetId )
	: AssetBaseInfo( assetType, fileName, fileLen, assetId )
{
}

//============================================================================
BlobInfo& BlobInfo::operator=( const BlobInfo& rhs ) 
{	
	if( this != &rhs )
	{
        *((AssetBaseInfo*)this) = rhs;
	}

	return *this;
}
