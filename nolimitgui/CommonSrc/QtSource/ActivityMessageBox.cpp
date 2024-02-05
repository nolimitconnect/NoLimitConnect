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

#include <CoreLib/ObjectCommonDefs.h>

#include <stdio.h>
#include <stdarg.h>
#include <array>

//============================================================================
ActivityMessageBox::ActivityMessageBox( AppCommon& app, QWidget* parent )
: ActivityBase( OBJNAME_ACTIVITY_MESSAGE_BOX, app, parent, eAppletMessengerFrame, false, true )
, m_OkButtonClicked( false )
{
	ui.setupUi(this);

	initMessageBoxCommon();
}

//============================================================================
ActivityMessageBox::ActivityMessageBox( AppCommon& app, QWidget* parent, int infoLevel, const char* msgFormat, ... )
: ActivityBase( OBJNAME_ACTIVITY_MESSAGE_BOX, app, parent, eAppletMessengerFrame, true )
, m_OkButtonClicked( false )
{
	ui.setupUi(this);

	initMessageBoxCommon();

	std::array<char, 4096> szBuffer;
	va_list arg_ptr;
	va_start(arg_ptr, msgFormat);
#ifdef TARGET_OS_WINDOWS
	vsnprintf(szBuffer.data(), 4096, msgFormat, (char*)arg_ptr);
#else
    vsnprintf(szBuffer.data(), 4096, msgFormat, arg_ptr);
#endif
	szBuffer.data()[4095] = 0;
	va_end(arg_ptr);

	setBodyText( szBuffer.data() );
}

//============================================================================
ActivityMessageBox::ActivityMessageBox( AppCommon& app, QWidget* parent, int infoLevel, QString msg )
: ActivityBase( OBJNAME_ACTIVITY_MESSAGE_BOX, app, parent, eAppletMessengerFrame, true )
, m_OkButtonClicked( false )
{
	ui.setupUi(this);

	initMessageBoxCommon();

	setBodyText( msg );
	this->setFocus();
}

//============================================================================
void ActivityMessageBox::initMessageBoxCommon( void )
{
	ui.m_TitleBarWidget->setTitleBarText( QObject::tr("Message") );
    connectBarWidgets();

	ui.m_ClipboardIconButton->setFixedSize( eButtonSizeMedium );
    ui.m_ClipboardIconButton->setIcon( eMyIconEditCopy );

	showCancelButton( false );
	connect( ui.m_AcceptCancelWidget,			SIGNAL(signalAccepted()), this, SLOT(onOkButClick()) );
	connect( ui.m_AcceptCancelWidget,			SIGNAL(signalCanceled()), this, SLOT(onCancelButClick()) );
	connect( ui.m_ClipboardButton,				SIGNAL(clicked()),		  this, SLOT(slotCopyToClipboardButtonClicked()) );
    connect( ui.m_ClipboardIconButton,			SIGNAL(clicked()),		  this, SLOT(slotCopyToClipboardButtonClicked()) );
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
	ui.m_AcceptCancelWidget->showCancelButton( showButton );
}

//============================================================================
void ActivityMessageBox::slotCopyToClipboardButtonClicked( void )
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText( ui.m_BodyTextLabel->text() );
    okMessageBox( QObject::tr( "Clipboard" ), QObject::tr( "Text was copied to clipboard" ) );
}
