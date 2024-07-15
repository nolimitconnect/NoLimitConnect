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
#include "IdentWidget.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>

#include <stdio.h>
#include <stdarg.h>
#include <array>

#include "ui_ActivityMessageBox.h"

TitleBarWidget *	ActivityMessageBox::getTitleBarWidget( void ) { return ui.m_TitleBarWidget; }
BottomBarWidget *	ActivityMessageBox::getBottomBarWidget( void ) { return ui.m_BottomBarWidget; }

//============================================================================
ActivityMessageBox::ActivityMessageBox( AppCommon& app, QWidget* parent )
: ActivityBase( OBJNAME_ACTIVITY_MESSAGE_BOX, app, parent, eAppletMessengerFrame, false, true )
, ui(*(new Ui::MessageBoxDialogClass))
{
	ui.setupUi(this);

	initMessageBoxCommon();
}

//============================================================================
ActivityMessageBox::ActivityMessageBox( AppCommon& app, QWidget* parent, QString title, QString msg )
: ActivityBase( OBJNAME_ACTIVITY_MESSAGE_BOX, app, parent, eAppletMessengerFrame, false, true )
, ui(*(new Ui::MessageBoxDialogClass))
{
	ui.setupUi(this);

	initMessageBoxCommon();

	ui.m_TitleBarWidget->setTitleBarText( title );
	setBodyText( msg );
	this->setFocus();
}

//============================================================================
ActivityMessageBox::ActivityMessageBox( AppCommon& app, QWidget* parent, int infoLevel, const char* msgFormat, ... )
: ActivityBase( OBJNAME_ACTIVITY_MESSAGE_BOX, app, parent, eAppletMessengerFrame, true )
, ui(*(new Ui::MessageBoxDialogClass))
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
, ui(*(new Ui::MessageBoxDialogClass))
{
	ui.setupUi(this);

	initMessageBoxCommon();

	setBodyText( msg );
}

//============================================================================
void ActivityMessageBox::initMessageBoxCommon( void )
{
	ui.m_IdentWidget->setVisible( false );
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

//============================================================================
void ActivityMessageBox::showOfferInfo( OfferBaseInfo& offerInfo )
{
	QString msg;
	VxGUID onlineId;
	if( eOfferMgrClient == offerInfo.getOfferMgr() )
	{
		onlineId = offerInfo.getSendToId();
	}
	else if( eOfferMgrHost == offerInfo.getOfferMgr() )
	{
		onlineId = offerInfo.getCreatorId();
	}
	else
	{
		setBodyText( QObject::tr( "Invalid Offer Manager" ) );
		return;
	}

	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( onlineId );
	if( !guiUser )
	{
		setBodyText( QObject::tr( "Unknown User" ) );
		return;
	}

	ui.m_IdentWidget->setVisible( true );
	ui.m_IdentWidget->updateIdentity( guiUser, false );
	msg = QObject::tr( "Offer: " ) + GuiParams::describeOfferType( offerInfo.getOfferType() );
	if( eOfferTypePersonFile == offerInfo.getOfferType() )
	{
		msg += QObject::tr( "\nFile: " ) + offerInfo.getFileInfo().getFileName().c_str();
		msg += QObject::tr( "\nFile Length: " ) + GuiParams::describeFileLength( offerInfo.getFileInfo().getFileLength() );
		msg += QObject::tr( "\nFile Type: " ) + GuiParams::describeFileType( offerInfo.getFileInfo().getFileType() );
	}

}
