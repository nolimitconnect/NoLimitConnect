//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiPlayerMgr.h"

#include "ActivityBase.h"
#include "AppCommon.h"
#include "AppletPlayerStream.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiPlayerCallback.h"
#include "HomeWindow.h"

#include <AssetMgr/AssetInfo.h>
#include <CoreLib/VxFileUtil.h>
#include <P2PEngine/P2PEngine.h>

namespace
{
	const int PROCESS_QT_DEFAULT_MS = 50;

	void ProcessQtEvents( int ms = PROCESS_QT_DEFAULT_MS )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents, ms );
	}
}

//============================================================================
GuiPlayerMgr::GuiPlayerMgr()
	: QObject()
{
}

//============================================================================
void GuiPlayerMgr::playerMgrStartup( void )
{
	connect( this, SIGNAL(signalInternalPlayVideoFrame(VxGUID,QImage*,int,int)), this, SLOT(slotInternalPlayVideoFrame(VxGUID,QImage*,int,int)), Qt::QueuedConnection );
	connect( this, SIGNAL(signalInternalPlayMotionVideoFrame(VxGUID,QImage*,int)), this, SLOT(slotInternalPlayMotionVideoFrame(VxGUID,QImage*,int)), Qt::QueuedConnection );
	// tell engine to send all video jpeg inputs
	VxGUID nullGuid;
	GetPtoPEngine().fromGuiWantMediaInput( nullGuid, eMediaInputVideoJpgSmall, eAppModuleMediaPlayer, true );
}

//============================================================================
void GuiPlayerMgr::wantPlayVideoCallbacks( VxGUID& feedOnlineId, GuiPlayerCallback* client, bool enable )
{
	if( m_VideoPlayClientsBusy )
	{
		LogMsg( LOG_ERROR, "GuiPlayerMgr::wantPlayVideoCallbacks do NOT call while busy" );
	}

	for( auto iter = m_VideoPlayClients.begin(); iter != m_VideoPlayClients.end(); ++iter )
	{
		if( iter->second == client &&  iter->first == feedOnlineId )
		{
			m_VideoPlayClients.erase( iter );
			break;
		}
	}

	if( enable )
	{
		m_VideoPlayClients.emplace_back( std::make_pair( feedOnlineId, client ) );
	}
}

//============================================================================
void GuiPlayerMgr::toGuiPlayVideoFrame( VxGUID& feedOnlineId, uint8_t* pu8Jpg, uint32_t u32JpgDataLen, int motion0To100000 )
{
	if( m_VideoPlayClients.empty() )
	{
		return;
	}

	if( !pu8Jpg || !u32JpgDataLen )
	{
		LogMsg( LOG_ERROR, " GuiPlayerMgr::%s invalid image data", __func__ );
		return;
	}

	int behindFramCnt = m_BehindMotionFrameCnt;
	if( behindFramCnt > 3 )
	{
		ProcessQtEvents( 50 );
		behindFramCnt = m_BehindMotionFrameCnt;
		if( behindFramCnt > 3 )
		{
			LogModule( eLogVideoIo, LOG_VERBOSE, " GuiPlayerMgr::%s behind frame cnt %d", __func__, behindFramCnt );
			return;
		}
	}

	QImage* vidFrame = new QImage();
	if( !vidFrame->loadFromData( pu8Jpg, u32JpgDataLen, "JPG") )
	{
		LogMsg( LOG_WARNING, "GuiPlayerMgr::%s failed to load JPG", __func__ );
		delete vidFrame;
		return;
	}

	m_BehindMotionFrameCnt++;
	emit signalInternalPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
}

//============================================================================
void GuiPlayerMgr::slotInternalPlayMotionVideoFrame( VxGUID feedOnlineId, QImage* vidFrame, int motion0To100000 )
{
	m_BehindMotionFrameCnt--;

	m_VideoPlayClientsBusy = true;
	for( auto clientPair : m_VideoPlayClients )
	{
		if( !clientPair.first.isVxGUIDValid() || clientPair.first == feedOnlineId )
		{
			clientPair.second->callbackGuiPlayMotionVideoFrame( feedOnlineId, *vidFrame, motion0To100000 );
		}
	}

	m_VideoPlayClientsBusy = false;
	delete vidFrame;
}

//============================================================================
int GuiPlayerMgr::toGuiPlayVideoFrame( VxGUID& feedOnlineId, uint8_t* picBuf, uint32_t picBufLen, int picWidth, int picHeight )
{
	int behindFramCnt = m_BehindMotionFrameCnt;
	if( m_VideoPlayClients.empty() )
	{
		return behindFramCnt;
	}

	if( !picBuf || picWidth < 10  || picHeight < 10 )
	{
		LogMsg( LOG_ERROR, " GuiPlayerMgr::%s invalid image data", __func__ );
		return behindFramCnt;
	}

	if( behindFramCnt > 3 )
	{
		ProcessQtEvents( 50 );
		behindFramCnt = m_BehindMotionFrameCnt;
		if( behindFramCnt > 3 )
		{
			LogModule( eLogVideoIo, LOG_VERBOSE, " GuiPlayerMgr::%s behind frame cnt %d", __func__, behindFramCnt );
			return behindFramCnt;
		}
	}

	QImage::Format imageFormat = QImage::Format_ARGB32;
	QImage* vidFrame = new QImage( picBuf, picWidth, picHeight, imageFormat );
	if( vidFrame->isNull() )
	{
		LogMsg( LOG_ERROR, " GuiPlayerMgr::%s invalid image data", __func__ );
		delete vidFrame;
		return behindFramCnt;
	}

	m_BehindFeedFrameCnt++;
	emit signalInternalPlayVideoFrame( feedOnlineId, vidFrame, picWidth, picHeight );

	return behindFramCnt;
}

//============================================================================
void GuiPlayerMgr::slotInternalPlayVideoFrame( VxGUID feedOnlineId, QImage* vidFrame, int picWidth, int picHeight )
{
	m_BehindFeedFrameCnt--;

	m_VideoPlayClientsBusy = true;
	for( auto clientPair : m_VideoPlayClients )
	{
		if( !clientPair.first.isVxGUIDValid() || clientPair.first == feedOnlineId )
		{
			clientPair.second->callbackGuiPlayVideoFrame( feedOnlineId, *vidFrame );
		}
	}

	m_VideoPlayClientsBusy = false;
	delete vidFrame;
}

//============================================================================
bool GuiPlayerMgr::playFile( QString fileNameAndPath, int pos0to100000, bool isStream, bool useExternPlayer )
{
    if( fileNameAndPath.isEmpty() )
	{
		LogMsg( LOG_WARNING, "GuiPlayerMgr::playFile Empty File Name" );
		return false;
	}

    VxFileInfoBase fileInfo;
    if( !VxFileUtil::getFileInfo( fileNameAndPath.toUtf8().constData(), fileInfo ) )
	{
        LogMsg( LOG_WARNING, "File no longer available %s", fileNameAndPath.toUtf8().constData() );
		return false;
	}

    LogMsg( LOG_VERBOSE, "%s file name %s file %s pos %d", __func__, fileInfo.getFileName().c_str(), fileInfo.getFileNameAndPath().c_str(), pos0to100000 );

    AssetInfo newAsset( fileInfo );
	newAsset.setIsStream( isStream );

	return playMedia( newAsset, useExternPlayer, pos0to100000 );
}

//============================================================================
bool GuiPlayerMgr::playStream( AssetBaseInfo& assetInfo, VxGUID lclSessionId, int pos0to100000 )
{
	return playMedia( assetInfo, false, pos0to100000 );
}

//============================================================================
bool GuiPlayerMgr::playMedia( AssetBaseInfo& assetInfo, bool useExternPlayer, int pos0to100000 )
{
	if( eAssetTypeExe == assetInfo.getAssetType() )
	{
		QMessageBox::warning( &GetAppInstance().getHomeWindow(), QObject::tr("Attempted to play an executable which is not allowed"), QString(assetInfo.getAssetName().c_str()));
		return false;
	}

	if( eAssetTypeArchives == assetInfo.getAssetType() )
	{
		QMessageBox::warning( &GetAppInstance().getHomeWindow(), QObject::tr( "Attempted to open an archive file which is not allowed" ), QString( assetInfo.getAssetName().c_str() ) );
		return false;
	}

	if( assetInfo.getIsStream() || !useExternPlayer )
	{
		if( assetInfo.isPhotoAsset() || assetInfo.getIsStream() || !GetAppInstance().getAppSettings().getUseSystemMediaPlayer() )
		{
			EApplet appletType = GuiHelpers::getAppletThatPlaysFile( GetAppInstance(), assetInfo );
			if( appletType != eAppletUnknown )
			{
				// launch the applet that plays this file
				ActivityBase* applet = GetAppInstance().launchApplet( appletType, &GetAppInstance().getHomeWindow(), "", assetInfo.getAssetUniqueId() );
				if( applet )
				{
					return applet->playMedia( assetInfo, pos0to100000 );
				}
			}
		}
	}

	if( !assetInfo.getIsStream() )
	{
#ifdef TARGET_OS_WINDOWS
		ShellExecuteA( 0, 0, assetInfo.getAssetName().c_str(), 0, 0, SW_SHOW );
#else
		QDesktopServices::openUrl( QUrl::fromLocalFile( assetInfo.getAssetName().c_str() ) );
#endif // TARGET_OS_WINDOWS
	}
	return true;
}

