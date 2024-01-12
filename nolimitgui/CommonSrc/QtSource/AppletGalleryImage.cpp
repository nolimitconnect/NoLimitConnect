//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletGalleryImage.h"

#include "AppCommon.h"	
#include "AppGlobals.h"
#include "AppSettings.h"
#include "FileShareItemWidget.h"
#include "AppletPopupMenu.h"
#include "FileItemInfo.h"
#include "FileActionMenu.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>
#include <AssetMgr/AssetMgr.h>

#include <PktLib/VxSearchDefs.h>
#include <NetLib/VxFileXferInfo.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileInfo.h>

//============================================================================
AppletGalleryImage::AppletGalleryImage(	AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_GALLERY_IMAGE, app, parent )
, m_AssetMgr( app.getEngine().getAssetMgr() )
{
    setAppletType( eAppletGalleryImage );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );



    m_MyApp.activityStateChange( this, true );
}

//============================================================================
void AppletGalleryImage::slotImageClicked( ThumbnailViewWidget * imageEntry )
{

}
