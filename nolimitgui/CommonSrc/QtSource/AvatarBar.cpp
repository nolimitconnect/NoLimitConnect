//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "AvatarBar.h"
#include "AppCommon.h"
#include "MyIcons.h"
#include "GuiParams.h"
#include "GuiUser.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxTimeUtil.h>

//============================================================================
AvatarBar::AvatarBar( QWidget* parent )
: QWidget( parent )
, m_MyApp( GetAppInstance() )
{
	ui.setupUi( this );

    ui.m_ShredButton->setSquareButtonSize( eButtonSizeSmall );
	ui.m_ProgressSpinner->setSquareButtonSize( eButtonSizeSmall );

	QSize buttonSize( GuiParams::getButtonSize( eButtonSizeSmall ) );
	
    ui.m_ResendButton->setFixedSize( buttonSize );
	ui.m_Avatar->setFixedSize( buttonSize );

	this->setFixedWidth( buttonSize.width() + 20 );

	setShredButtonIcon( eMyIconShredderNormal  );
	ui.m_ProgressSpinner->setVisible( false );
	ui.m_ShredButton->setVisible( false );
	ui.m_TimeLabel->setVisible( false );
	ui.m_ResendButton->setVisible( false );

	connect( ui.m_ShredButton, SIGNAL(clicked()),	this, SIGNAL(signalShredAsset()) );
	connect( ui.m_ResendButton, SIGNAL(clicked()),	this, SIGNAL(signalResendAsset()) );
}

//============================================================================
void AvatarBar::setTime( time_t creationTime, bool isQueued )
{
	if( isQueued )
	{
		ui.m_TimeLabel->setText( QObject::tr( "queued" ) );
	}
	else
	{
		std::string timeStr = VxTimeUtil::formatTimeStampIntoHoursAndMinutes( creationTime );
		ui.m_TimeLabel->setText( timeStr.c_str() );
	}

	ui.m_TimeLabel->setVisible( true );
	showAvatar( false );
}

//============================================================================
void AvatarBar::setOnlineId( VxGUID& onlineId )
{
	m_OnlineId = onlineId;
	if( !m_ThumbnailQueried )
	{
		m_ThumbnailQueried = true;
		GuiUser* guiUser = m_MyApp.getUserMgr().getOrQueryUser( onlineId );
		if( guiUser )
		{
			QImage image;
			VxGUID thumbId = guiUser->getAvatarThumbId();
			if( thumbId.isVxGUIDValid() && m_MyApp.getThumbMgr().getAvatarImage( thumbId, image ) )
			{
				ui.m_Avatar->setImage( image );
				update();
			}
		
		}
	}
}

//============================================================================
void AvatarBar::showAvatar( bool show )
{
	ui.m_Avatar->setVisible( show );
	update();
}

//============================================================================
void AvatarBar::setShredButtonIcon( EMyIcons iconTrash )
{
	ui.m_ShredButton->setIcon( iconTrash );
}

//============================================================================
void AvatarBar::setShredFile( QString fileName )
{
	//ui.m_ShredButton->setShredFile( fileName );
}

//============================================================================
void AvatarBar::showSendFail( bool show, bool permissionErr )
{
	ui.m_Avatar->setVisible( show );
	if( show )
	{
		if( permissionErr )
		{
			ui.m_Avatar->setImage( ":/AppRes/Resources/msg_state_fail_permission.png" );
		}
		else
		{
			ui.m_Avatar->setImage( ":/AppRes/Resources/msg_state_fail_resend.png" );
		}
	}

	if( permissionErr )
	{
		showResendButton( false );
	}
	else
	{
		showResendButton( show );
	}
}

//============================================================================
void AvatarBar::showResendButton( bool show )
{
	ui.m_ResendButton->setVisible( show );
	if( show )
	{
		ui.m_ResendButton->setIcon( eMyIconSendArrowNormal );
	}
}

//============================================================================
void AvatarBar::showShredder( bool show )
{
	ui.m_ShredButton->setVisible( show );
}

//============================================================================
void AvatarBar::showXferProgress( bool show )
{
	ui.m_ProgressSpinner->setVisible( show );
	if( show )
	{
		ui.m_ProgressSpinner->setImage( ":/AppRes/Resources/spinner-clockwise.svg"  );
	}
}

//============================================================================
void AvatarBar::setXferProgress( int xferProgress )
{
	ui.m_ProgressSpinner->setValue( xferProgress );
}

//============================================================================
int AvatarBar::getDesiredHeight( void )
{
	int desiredHeight{ 0 };
	desiredHeight += ui.m_Avatar->isVisible() ? ui.m_Avatar->height() + 2 : 0;
	desiredHeight += ui.m_ProgressSpinner->isVisible() ? ui.m_ProgressSpinner->height() + 2 : 0;
	desiredHeight += ui.m_ShredButton->isVisible() ? ui.m_ShredButton->height() + 2 : 0;
	desiredHeight += ui.m_ResendButton->isVisible() ? ui.m_ResendButton->height() + 2 : 0;
	desiredHeight += ui.m_TimeLabel->isVisible() ? ui.m_TimeLabel->height() + 2 : 0;

	if( !desiredHeight )
	{
		// always leve room for at least trash can and time
		desiredHeight = ui.m_ShredButton->height() + ui.m_TimeLabel->height();
	}

	return desiredHeight;
}