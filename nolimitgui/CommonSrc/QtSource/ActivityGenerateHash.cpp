//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityGenerateHash.h"
#include "MyIcons.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "GuiHelpers.h"

#include <PktLib/VxCommon.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/Sha1GeneratorMgr.h>

#include "ui_ActivityGenerateHash.h"

TitleBarWidget *  ActivityGenerateHash::getTitleBarWidget( void ) { return ui.m_TitleBarWidget; }
BottomBarWidget * ActivityGenerateHash::getBottomBarWidget( void ) { return ui.m_BottomBarWidget; }

//============================================================================
ActivityGenerateHash::ActivityGenerateHash(	AppCommon& app, QWidget* parent, std::string fileName, VxSha1Hash& hashId )
: ActivityBase( OBJNAME_ACTIVITY_GENERATE_HASH, app, parent, eAppletMessengerFrame, true )
, ui(*(new Ui::ActivityGenerateHashUi))
, m_FileName( fileName )
, m_HashId( hashId )
{
	ui.setupUi(this);

    connectBarWidgets();

	ui.m_CancelButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_CancelButton->setIcon( eMyIconCancelNormal );

    connect(ui.m_TitleBarWidget, SIGNAL(signalBackButtonClicked()), this, SLOT(slotHomeButtonClicked()));

	connect( ui.m_CancelButton, SIGNAL(clicked()), this, SLOT( slotCancelButtonClicked()) );
	this->setFocus();
	ui.m_CancelButton->setFocus();

	QString	title( QObject::tr("Generate Hash"));
	QString	bodyText( QObject::tr("Please wait while hash is generated.\nThis message will disappear when generate hash is completed."));

	setTitle( title );
	ui.m_BodyTextLabel->setText( bodyText );

	m_HashInstanceId.initializeWithNewVxGUID();
	GetSha1GeneratorMgr().generateSha1( m_HashInstanceId, m_FileName, this );
}

//============================================================================
void ActivityGenerateHash::setTitle( QString strTitle )
{
	ui.m_TitleBarWidget->setTitleBarText(strTitle);
}

//============================================================================
void ActivityGenerateHash::setBodyText( QString strBodyText )
{
	ui.m_BodyTextLabel->setText( strBodyText );
}

//============================================================================
void ActivityGenerateHash::hideCancelButton( void )
{
	ui.m_CancelButton->setVisible( false );
}

//============================================================================
void ActivityGenerateHash::slotHomeButtonClicked( void )
{
	GetSha1GeneratorMgr().cancelGenerateSha1( m_HashInstanceId, m_FileName, this );
	reject();
}

//============================================================================
void ActivityGenerateHash::slotCancelButtonClicked( void )
{
	GetSha1GeneratorMgr().cancelGenerateSha1( m_HashInstanceId, m_FileName, this );
	reject();
}

//============================================================================
void ActivityGenerateHash::slotGenerateSha1Completed( ESha1GenResult sha1GenResult )
{
	if( eSha1GenResultNoError == sha1GenResult )
	{
		accept();
	}
	else
	{
		// TODO should warn user the generate has failed
		LogMsg( LOG_ERROR, "ActivityGenerateHash gen hash failed %s", DescribeSha1GenResult( sha1GenResult ) );
		reject();
	}
}

//============================================================================
void ActivityGenerateHash::callbackSha1GenerateResult( ESha1GenResult sha1GenResult, VxGUID& fileId, Sha1Info& sha1Info )
{
	if( fileId == m_HashInstanceId )
	{
		if( eSha1GenResultNoError == sha1GenResult )
		{
			m_HashId = sha1Info.getSha1Hash();
		}

		// this callback is from a different thread so use signal/slot
		emit signalGenerateSha1Completed( sha1GenResult );
	}
}
