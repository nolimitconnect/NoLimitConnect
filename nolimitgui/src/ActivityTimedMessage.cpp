//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityTimedMessage.h"

#include "ui_ActivityTimedMessage.h"

//============================================================================
ActivityTimedMessage::ActivityTimedMessage( QString strTitle, QString strMessage, int iTimeoutSeconds, QWidget* parent )
	: QDialog( parent, Qt::SubWindow )
	, ui(*(new Ui::TimedMessageDlg))
{
	ui.setupUi(this);
	QRect oRect = parent->geometry();
	oRect.setRight(oRect.right() - oRect.left());
	oRect.setLeft(0);
	oRect.setBottom(oRect.bottom() - oRect.top());
	oRect.setTop(0);
	this->setGeometry(oRect);


	connect( ui.m_TitleBarWidget, SIGNAL(signalBackButtonClicked()), this, SLOT(reject()) );

	this->ui.m_TitleBarWidget->setTitleBarText( strTitle );
	this->ui.LogEdit->append(strMessage);
	if( iTimeoutSeconds )
	{
		connect( &m_CloseDlgTimer, SIGNAL(timeout()), this, SLOT(accept()) );
		m_CloseDlgTimer.setSingleShot( true );
		m_CloseDlgTimer.start( iTimeoutSeconds * 1000 );
	}
}

