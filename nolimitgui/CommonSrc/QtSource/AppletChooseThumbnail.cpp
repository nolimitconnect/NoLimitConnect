//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletChooseThumbnail.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "AppGlobals.h"
#include "AccountMgr.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <QMessageBox>
#include <QUuid>
#include <QFileDialog>

#include <P2PEngine/EngineSettings.h>
#include <AssetMgr/AssetMgr.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
AppletChooseThumbnail::AppletChooseThumbnail( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_CHOOSE_THUMBNAIL, app, parent )
{
    setAppletType( eAppletChooseThumbnail );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    QString infoText = QObject::tr( "Image for thumbnail will be resized to width " );
    infoText += QString::number( GuiParams::getThumbnailSize().width() );
    infoText += QObject::tr( " height " );
    infoText += QString::number( GuiParams::getThumbnailSize().height() );
    infoText += QObject::tr( " pixels. " );
    ui.m_InfoLabel->setText( infoText );

    connect( ui.m_ThumbnailEditWidget, SIGNAL( signalImageChanged() ), this, SLOT( slotImageChanged() ) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletChooseThumbnail::~AppletChooseThumbnail()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletChooseThumbnail::slotImageChanged( void )
{
    emit signalThumbSelected( this, ui.m_ThumbnailEditWidget );
}
