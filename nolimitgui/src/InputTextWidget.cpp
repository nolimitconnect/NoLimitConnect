//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InputTextWidget.h"
#include "AppCommon.h"
#include "GuiParams.h"
#include "GuiHelpers.h"
#include "InputClientBaseCallback.h"

#include <P2PEngine/P2PEngine.h>

#include <QEvent>
#include <QKeyEvent>

#include <time.h>

#include "ui_InputTextWidget.h"

//============================================================================
InputTextWidget::InputTextWidget( QWidget* parent )
: InputBaseWidget( GetAppInstance(), parent )
, ui(*(new Ui::InputTextWidgetClass))
{
	m_AssetInfo.setAssetType( eAssetTypeChatText );

	ui.setupUi( this );
    ui.m_CancelTextButton->setSquareButtonSize( eButtonSizeTiny );
    ui.m_SendTextButton->setSquareButtonSize( eButtonSizeTiny );

	ui.m_CancelTextButton->setIcons( eMyIconSendArrowCancel );
	ui.m_SendTextButton->setIcons( eMyIconSendArrowNormal );

	connect( ui.m_CancelTextButton, SIGNAL(clicked()), this, SLOT(slotCancelButtonClicked()) );
	connect( ui.m_SendTextButton, SIGNAL(clicked()), this, SLOT(slotSendButtonClicked()) );
	setFocusProxy( ui.m_ChatTextEdit );
	ui.m_ChatTextEdit->installEventFilter( this );
	ui.m_ChatTextEdit->setFocus();
}

//============================================================================
void InputTextWidget::setFocusOnText( void )
{
	ui.m_ChatTextEdit->setFocus();
}

//============================================================================
bool InputTextWidget::eventFilter( QObject* watched, QEvent* event )
{
	if( ( watched == ui.m_ChatTextEdit ) && ( QEvent::KeyPress == event->type() ) )
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );
		if( ( Qt::Key_Return == keyEvent->key() ) || ( Qt::Key_Enter == keyEvent->key() ) )
		{
		#ifdef TARGET_OS_ANDROID
			// On mobile virtual keyboards, Enter is expected to insert a newline.
			return false;
		#else
			if( keyEvent->modifiers().testFlag( Qt::ShiftModifier ) )
			{
				return false;
			}

			slotSendButtonClicked();
			return true;
		#endif
		}
	}

	return InputBaseWidget::eventFilter( watched, event );
}

//============================================================================
void InputTextWidget::slotCancelButtonClicked( void )
{
	emit signalInputCompleted();
}

//============================================================================
void InputTextWidget::slotSendButtonClicked( void )
{
    if( m_GroupieId.isHostedIdValid() )
	{
		// What did they want to say (minus white space around the string):
		QString message = ui.m_ChatTextEdit->toPlainText().trimmed();

		// Only send the text if it's not empty
		if( !message.isEmpty() && fillAssetBaseInfo( true ) )
		{
			message += "\n";
			m_AssetInfo.setAssetName( message.toUtf8().constData() );
			m_AssetInfo.setAssetLength( message.length() );

			m_ClientCallback->handleAssetAction( m_IsPersonalRecorder ? eAssetActionAddToAssetMgr : eAssetActionAddAssetAndSend, m_AssetInfo );
		}

		ui.m_ChatTextEdit->clear();

		// Put the focus back into the input box so they can type again:
		ui.m_ChatTextEdit->setFocus();
	}
	else
	{
		GuiHelpers::errorMsgBox( eErrMsgNoUserSelectedToSendTo, this );
	}

	emit signalInputCompleted();
}
