//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityMessageBox.h"
#include "MyIcons.h"
#include "AppGlobals.h"
#include "AppCommon.h"

#include <P2PEngine/P2PEngine.h>

#include <stdio.h>
#include <stdarg.h>

//============================================================================
ActivityMessageBox::ActivityMessageBox( AppCommon& app, QWidget* parent )
: ActivityBase( OBJNAME_ACTIVITY_MESSAGE_BOX, app, parent, eAppletMessengerFrame, false, true )
, m_OkButtonClicked( false )
{
	ui.setupUi(this);
	ui.m_TitleBarWidget->setTitleBarText( QObject::tr("Message") );

    connectBarWidgets();

	connect( ui.m_OkButton,					SIGNAL(clicked()), this, SLOT(onOkButClick()) );
	connect( ui.m_CancelButton,				SIGNAL(clicked()), this, SLOT(onCancelButClick()) );
	connect( ui.m_TitleBarWidget,			SIGNAL(signalBackButtonClicked()), this, SLOT(onCancelButClick()) );
}

//============================================================================
ActivityMessageBox::ActivityMessageBox( AppCommon& app, QWidget* parent, int infoLevel, const char* msgFormat, ... )
: ActivityBase( OBJNAME_ACTIVITY_MESSAGE_BOX, app, parent, eAppletMessengerFrame, true )
, m_OkButtonClicked( false )
{
	ui.setupUi(this);
	ui.m_TitleBarWidget->setTitleBarText( QObject::tr("Message") );

    connectBarWidgets();

	connect( ui.m_OkButton,					SIGNAL(clicked()), this, SLOT(onOkButClick()) );
	//connect( ui.m_CancelButton,				SIGNAL(clicked()), this, SLOT(onCancelButClick()) );
	ui.m_CancelButton->setVisible( false );
	connect( ui.m_TitleBarWidget,			SIGNAL(signalBackButtonClicked()), this, SLOT(onCancelButClick()) );

	char szBuffer[4096];
	va_list arg_ptr;
	va_start(arg_ptr, msgFormat);
#ifdef TARGET_OS_WINDOWS
	vsnprintf(szBuffer, 4096, msgFormat,(char *) arg_ptr);
#else
    vsnprintf(szBuffer, 4096, msgFormat, arg_ptr);
#endif
	szBuffer[4095] = 0;
	va_end(arg_ptr);

	setBodyText( szBuffer );
	this->setFocus();
	ui.m_OkButton->setFocus();
}

//============================================================================
ActivityMessageBox::ActivityMessageBox( AppCommon& app, QWidget* parent, int infoLevel, QString msg )
: ActivityBase( OBJNAME_ACTIVITY_MESSAGE_BOX, app, parent, eAppletMessengerFrame, true )
, m_OkButtonClicked( false )
{
	ui.setupUi(this);
	ui.m_TitleBarWidget->setTitleBarText( QObject::tr("Message") );
    connectBarWidgets();

	connect( ui.m_OkButton,					SIGNAL(clicked()), this, SLOT(onOkButClick()) );
	//connect( ui.m_CancelButton,				SIGNAL(clicked()), this, SLOT(onCancelButClick()) );
	ui.m_CancelButton->setVisible( false );
	connect( ui.m_TitleBarWidget,			SIGNAL(signalBackButtonClicked()), this, SLOT(onCancelButClick()) );

	setBodyText( msg );
	this->setFocus();
	ui.m_OkButton->setFocus();
}

//============================================================================
void ActivityMessageBox::onOkButClick( void )
{
	m_OkButtonClicked = true;
	m_ResultButton = QMessageBox::Ok;
	accept();
}

//============================================================================
void ActivityMessageBox::onCancelButClick( void )
{
	m_ResultButton = QMessageBox::Cancel;
	reject();
}

//============================================================================
void ActivityMessageBox::setTitleText( QString titleText )
{
	ui.m_TitleBarWidget->setTitleBarText( titleText );
}

//============================================================================
void ActivityMessageBox::setBodyText( QString bodyText )
{
	ui.m_BodyTextLabel->setText( bodyText );
}

//============================================================================
void ActivityMessageBox::showCancelButton( bool showButton )
{
	ui.m_CancelButton->setVisible( showButton );
}
