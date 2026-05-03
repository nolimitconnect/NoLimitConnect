//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileItemInfo.h"

#include "AppCommon.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>
#include <AssetMgr/AssetMgr.h>
#include <AssetMgr/AssetInfo.h>

#include <PktLib/VxSearchDefs.h>
#include <PktLib/VxCommon.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxFileIsTypeFunctions.h>
#include <CoreLib/VxFileUtil.h>

#include <string>

//============================================================================
FileItemInfo::FileItemInfo( FileInfo& fileInfo )
: m_FileInfo( fileInfo )
{
}

//============================================================================
QString FileItemInfo::describeFileType()
{
    return GuiParams::describeFileType( getFileType() );
}

//============================================================================
QString FileItemInfo::describeFileLength()
{
    return GuiParams::describeFileLength( getFileLength() );
}

//============================================================================
bool FileItemInfo::toggleIsShared( void )
{
    m_FileInfo.setIsSharedFile( !m_FileInfo.getIsSharedFile() );
    return m_FileInfo.getIsSharedFile();
}

//============================================================================
bool FileItemInfo::toggleIsInLibrary( void )
{
    m_FileInfo.setIsInLibrary( !m_FileInfo.getIsInLibrary() );
    return m_FileInfo.getIsInLibrary();
}

//============================================================================
void FileItemInfo::updateWidget( void )
{
	if( 0 != m_Widget )
	{
		m_Widget->update();
	}
}

//============================================================================
bool FileItemInfo::shouldOpenFile( void )
{
	return VxShouldOpenFile( getFileType() );
}

//============================================================================
bool FileItemInfo::toAsssetInfo( AppCommon& myApp, AssetBaseInfo& assetInfoOut, bool* retIsNewAsset )
{
    bool result{ false };
    if( retIsNewAsset )
    {
        *retIsNewAsset = false;
    }

    std::string assetFileName = getFileNameAndPath().toUtf8().constData();
    VxGUID assetId = getAssetId();

    if( VXFILE_TYPE_DIRECTORY == getFileType() )
    {
        // convert to asset but do not add to asset manager and return false
    }
    else
    {
        // determine asset id for this file.. may be different than the one given       
        int64_t fileLen = VxFileUtil::fileExists( assetFileName.c_str() );
        if( fileLen )
        {
            AssetBaseInfo* assetInfo = myApp.getEngine().getAssetMgr().findAsset( assetFileName );
            if( assetInfo && assetInfo->getAssetUniqueId().isValid() )
            {
                assetId = assetInfo->getAssetUniqueId();
                setAssetId( assetId );

                if( fileLen != assetInfo->getAssetLength() )
                {
                    assetInfo->updateAssetLength( fileLen );
                }

                assetInfoOut = *assetInfo;
                result = true;
            }
            else
            {
                if( !assetId.isValid() )
                {
                    assetId.initializeWithNewVxGUID();
                }

                AssetInfo newAsset( m_FileInfo );
                newAsset.setAssetUniqueId( assetId );
                assetInfoOut = newAsset;
                if( retIsNewAsset )
                {
                    *retIsNewAsset = true;
                }

                result = true;
            }
        }
    }

    return result;
}