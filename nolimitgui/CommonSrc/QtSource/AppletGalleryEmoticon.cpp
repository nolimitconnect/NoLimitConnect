//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"	
#include "AppSettings.h"

#include "AppletGalleryEmoticon.h"

#include "FileShareItemWidget.h"
#include "MyIcons.h"
#include "AppletPopupMenu.h"
#include "AppGlobals.h"
#include "FileItemInfo.h"
#include "FileActionMenu.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>
#include <ThumbMgr/ThumbInfo.h>
#include <ThumbMgr/ThumbMgr.h>

#include <PktLib/VxSearchDefs.h>
#include <NetLib/VxFileXferInfo.h>
#include <CoreLib/VxFileInfo.h>
#include <CoreLib/VxGlobals.h>

#include <QResizeEvent>

//============================================================================
AppletGalleryEmoticon::AppletGalleryEmoticon(	AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_GALLERY_EMOTICON, app, parent )
{
    setAppletType( eAppletGalleryEmoticon );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    connect( ui.m_ImageListWidget, SIGNAL( signalImageClicked( ThumbnailViewWidget * ) ), this, SLOT( slotImageClicked( ThumbnailViewWidget * ) ) );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletGalleryEmoticon::~AppletGalleryEmoticon()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletGalleryEmoticon::resizeEvent( QResizeEvent* ev )
{
    AppletBase::resizeEvent( ev );
    //LogMsg( LOG_DEBUG, "AppletGalleryEmoticon Resize w %d h %d\n", ev->size().width(), ev->size().height() );
    if( m_isShown && !m_isLoaded )
    {
        m_isLoaded = true;
        loadAssets();
    }
}

//============================================================================
void AppletGalleryEmoticon::showEvent( QShowEvent* ev )
{
    //LogMsg( LOG_DEBUG, "AppletGalleryEmoticon show event\n" );
    m_isShown = true;
    AppletBase::showEvent( ev );
}

//============================================================================
void AppletGalleryEmoticon::loadAssets( void )
{
    m_MyApp.getThumbMgr().generateEmoticonsIfNeeded( this );
    int emoticonNum = 1;
    std::vector<VxGUID>& emoticonIdList = ThumbMgr::getEmoticonIdList();
    for( auto& assetId : emoticonIdList )
    {
        if( assetId.isVxGUIDValid() )
        {
            AssetBaseInfo* assetInfo = m_MyApp.getEngine().getThumbMgr().findAsset(assetId);
            if( assetInfo && eAssetTypeThumbnail == assetInfo->getAssetType() )
            {
                LogMsg( LOG_VERBOSE, "AppletGalleryEmoticon::loadAssets emoticon %d is %s", emoticonNum, assetId.toOnlineIdString().c_str() );
                ui.m_ImageListWidget->addAsset( dynamic_cast<ThumbInfo*>(assetInfo) );
            }
            else
            {
                LogMsg( LOG_ERROR, "AppletGalleryEmoticon::loadAssets emoticon %d not found %s", emoticonNum, assetId.toOnlineIdString().c_str() );
            }
        }
        else
        {
            LogMsg( LOG_ERROR, "AppletGalleryEmoticon::loadAssets emoticon %d invalid %s", emoticonNum, assetId.toOnlineIdString().c_str() );
        }

        emoticonNum++;
    }
}

//============================================================================
void AppletGalleryEmoticon::slotImageClicked( ThumbnailViewWidget * thumb )
{
    if( thumb )
    {
        emit signalThumbSelected( this, thumb );
    }
}
