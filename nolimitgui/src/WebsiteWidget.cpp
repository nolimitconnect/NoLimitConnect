//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "WebsiteWidget.h"

#include "AppCommon.h"
#include "GuiParams.h"
#include "VxPushButton.h"

#include <QDesktopServices>

#include "ui_WebsiteWidget.h"

//============================================================================
WebsiteWidget::WebsiteWidget( QWidget* parent )
: QFrame( parent )
, ui(*(new Ui::WebsiteWidgetUi))
, m_MyApp(GetAppInstance())
{
	ui.setupUi( this );
	VxPushButton* actionButton = ui.m_WebsiteButton;

	actionButton->setFixedSize( eButtonSizeMedium );
	actionButton->setUseTheme( false );
	actionButton->setProperty( "NoLimitConnectIcon", true );
	actionButton->setAppIcon( eMyIconApp, parent );

	connect( ui.m_WebsiteButton, SIGNAL(clicked()), this, SLOT(slotGoToWebsite()) );
	connect( ui.m_InstructionLabel, SIGNAL(clicked()), this, SLOT(slotGoToWebsite()) );
	connect( ui.m_UrlLabel, SIGNAL(clicked()), this, SLOT(slotGoToWebsite()) );

	setUrlType( eWebsiteUrlNlc );
}

//============================================================================
void WebsiteWidget::setUrlType( WebsiteUrlType urlType )
{
	switch( urlType )
	{
	case eWebsiteUrlVpn:
		m_WebsiteUrl = "https://nolimitconnect.com/nlc/vpns/";
		break;

	default:
		m_WebsiteUrl = "https://nolimitconnect.com/";
	}

	QColor linkColor = m_MyApp.getAppTheme().getLinkColor( false );
	QString urlText = m_MyApp.getAppTheme().getFontBeginTag( linkColor ) + m_WebsiteUrl + m_MyApp.getAppTheme().getFontEndTag();
	ui.m_UrlLabel->setText( urlText );
}

//============================================================================
void WebsiteWidget::slotGoToWebsite( void )
{
	QUrl url( m_WebsiteUrl );
	QDesktopServices::openUrl( url );
}


