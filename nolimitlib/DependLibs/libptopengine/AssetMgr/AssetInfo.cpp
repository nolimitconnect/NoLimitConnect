//============================================================================
// Copyright (C) 2015 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AssetInfo.h"

#include <CoreLib/VxFileLists.h>
#include <CoreLib/VxFileIsTypeFunctions.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <sys/types.h>
#include <sys/stat.h>

//============================================================================
AssetInfo::AssetInfo()
: AssetBaseInfo()
{ 
}

//============================================================================
AssetInfo::AssetInfo( EAssetType assetType )
    : AssetBaseInfo( assetType )
{ 
}

//============================================================================
AssetInfo::AssetInfo( const AssetInfo& rhs )
    : AssetBaseInfo(rhs)
{
	*this = rhs;
}

//============================================================================
AssetInfo::AssetInfo( const AssetBaseInfo& rhs )
{
    *((AssetBaseInfo*)this) = rhs;
}

//============================================================================
AssetInfo::AssetInfo( FileInfo& rhs )
	: AssetBaseInfo( rhs )
{
}

//============================================================================
AssetInfo::AssetInfo( EAssetType assetType, std::string fileName )
: AssetBaseInfo( assetType, fileName )
{ 
}

//============================================================================
AssetInfo::AssetInfo( EAssetType assetType, std::string fileName, VxGUID& assetId )
	: AssetBaseInfo( assetType, fileName, assetId )
{
}

//============================================================================
AssetInfo::AssetInfo( EAssetType assetType, std::string fileName, uint64_t fileLen )
: AssetBaseInfo( assetType, fileName, fileLen )
{
}

//============================================================================
AssetInfo::AssetInfo( EAssetType assetType, std::string fileName, uint64_t fileLen, VxGUID& assetId )
	: AssetBaseInfo( assetType, fileName, fileLen, assetId )
{
}

//============================================================================
AssetInfo& AssetInfo::operator=( const AssetInfo& rhs ) 
{	
	if( this != &rhs )
	{
        *( (AssetBaseInfo*)this) =  *( (AssetBaseInfo*)&rhs);
	}

	return *this;
}
