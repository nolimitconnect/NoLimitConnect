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
#include "AppletPlayerNlc.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiPlayerCallback.h"
#include "GuiVideoTitleBarCallback.h"
#include "HomeWindow.h"

#include <AssetMgr/AssetInfo.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <P2PEngine/P2PEngine.h>

#include <QDesktopServices>
#include <QUrl>
#include <QPainter>

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
	m_MediaSessionId.initializeWithNewVxGUID();
}

//============================================================================
void GuiPlayerMgr::playerMgrStartup( void )
{
	connect( this, SIGNAL(signalInternalPlayVideoFrame(VxGUID,QImage*,int,int)), this, SLOT(slotInternalPlayVideoFrame(VxGUID,QImage*,int,int)), Qt::QueuedConnection );
	// the SharedUint8DataPtr forces use of new style connect or does not pass it through signals and slots
	QObject::connect( this, &GuiPlayerMgr::signalInternalPlayMotionVideoFrame, this, &GuiPlayerMgr::slotInternalPlayMotionVideoFrame, Qt::QueuedConnection );
	// tell engine to send all video jpeg inputs
	VxGUID nullGuid;
	GetPtoPEngine().fromGuiWantMediaInput( nullGuid, eMediaInputVideoJpgSmall, eAppModuleMediaPlayer, m_MediaSessionId, true );
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
void GuiPlayerMgr::wantVideoTitleBarCallbacks( GuiVideoTitleBarCallback* client, bool enable )
{
	for( auto iter = m_VideoTitleBarClients.begin(); iter != m_VideoTitleBarClients.end(); ++iter )
	{
		if( *iter == client )
		{
			m_VideoTitleBarClients.erase( iter );
			break;
		}
	}

	if( enable )
	{
		m_VideoTitleBarClients.emplace_back( client );
	}
}

//============================================================================
void GuiPlayerMgr::toGuiPlayVideoFrame( VxGUID& feedOnlineId, uint8_t* pu8Jpg, uint32_t u32JpgDataLen, int motion0To100000 )
{
	if( m_VideoTitleBarClients.empty() && m_VideoPlayClients.empty() )
	{
		return;
	}

	if( !pu8Jpg || !u32JpgDataLen )
	{
		LogMsg( LOG_ERROR, " GuiPlayerMgr::%s invalid image data", __func__ );
		return;
	}

	if( m_BehindMotionFrameCnt > 1 )
	{
		return;
	}

	// unfortunately using QImage causes max QImage used error on android so have to do shared data instead
	uint8_t* jpgData = new uint8_t[u32JpgDataLen];
	memcpy( jpgData, pu8Jpg, u32JpgDataLen );
	SharedUint8DataPtr vidData = SharedUint8DataPtr(jpgData);
	//QImage* vidFrame = new QImage();
	//if( !vidFrame->loadFromData( pu8Jpg, u32JpgDataLen, "JPG") )
	//{
	//	LogMsg( LOG_WARNING, "GuiPlayerMgr::%s failed to load JPG", __func__ );
	//	delete vidFrame;
	//	return;
	//}

	m_BehindMotionFrameCnt++;
	emit signalInternalPlayMotionVideoFrame( feedOnlineId, vidData, u32JpgDataLen, motion0To100000 );
}

//============================================================================
void GuiPlayerMgr::slotInternalPlayMotionVideoFrame( VxGUID feedOnlineId, SharedUint8DataPtr vidData, int dataLen, int motion0To100000 )
{
	m_BehindMotionFrameCnt--;

	QImage* vidFrame = new QImage();
	if( !vidFrame->loadFromData( vidData.get(), dataLen, "JPG") )
	{
		LogMsg( LOG_WARNING, "GuiPlayerMgr::%s failed to load JPG", __func__ );
		delete vidFrame;
		return;
	}

	vx_assert( !m_VideoPlayClientsBusy );

	m_VideoPlayClientsBusy = true;
	for( auto clientPair : m_VideoPlayClients )
	{
		if( !clientPair.first.isVxGUIDValid() || clientPair.first == feedOnlineId )
		{
			clientPair.second->callbackGuiPlayMotionVideoFrame( feedOnlineId, *vidFrame, motion0To100000 );
		}
	}

	m_VideoPlayClientsBusy = false;

	if( m_VideoTitleBarClients.size() && feedOnlineId == GetAppInstance().getMyOnlineId() && m_TitleBarImageSize.width() )
	{
		QPixmap titleBarPixmap = QPixmap::fromImage( vidFrame->scaled( m_TitleBarImageSize ) );
		if( !titleBarPixmap.isNull() )
		{
			for( auto client : m_VideoTitleBarClients )
			{
				client->callbackGuiVideoTitleBarPixmap( titleBarPixmap );
			}
		}
	}

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

	if( behindFramCnt > 2 )
	{
		LogModule( eLogVideoIo, LOG_VERBOSE, " GuiPlayerMgr::%s behind frame cnt %d", __func__, behindFramCnt );
		return behindFramCnt;
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
	// launch the applet that plays this file
	ActivityBase* applet = GetAppInstance().launchApplet( eAppletPlayerNlc, &GetAppInstance().getHomeWindow(), "", assetInfo.getAssetUniqueId() );
	if( applet )
	{
		AppletPlayerNlc* player = dynamic_cast<AppletPlayerNlc*>(applet);
		if( player )
		{
			AssetPlaySession playSession( assetInfo, lclSessionId, pos0to100000 );
			return player->playMedia( playSession, false );
		}		
	}

	return false;
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

    if( assetInfo.isStream() || !useExternPlayer )
	{
        if( assetInfo.isPhotoAsset() || assetInfo.isStream() || !GetAppInstance().getAppSettings().getUseSystemMediaPlayer() )
		{
			EApplet appletType = GuiHelpers::getAppletThatPlaysFile( GetAppInstance(), assetInfo );
			if( appletType != eAppletUnknown )
			{
				// launch the applet that plays this file
				ActivityBase* applet = GetAppInstance().launchApplet( appletType, &GetAppInstance().getHomeWindow(), "", assetInfo.getAssetUniqueId() );
				if( applet )
				{
					AssetPlaySession assetPlaySession( assetInfo );
					assetPlaySession.setPlayPosition( pos0to100000 );
					return applet->playMedia( assetPlaySession, false );
				}
			}
		}
	}

    if( !assetInfo.isStream() )
	{
#ifdef TARGET_OS_WINDOWS
		ShellExecuteA( 0, 0, assetInfo.getAssetNameAndPath().c_str(), 0, 0, SW_SHOW );
#else
		QDesktopServices::openUrl( QUrl::fromLocalFile( assetInfo.getAssetNameAndPath().c_str() ) );
#endif // TARGET_OS_WINDOWS
	}
	return true;
}

