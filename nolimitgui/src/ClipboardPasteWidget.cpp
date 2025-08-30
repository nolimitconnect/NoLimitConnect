//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ClipboardPasteWidget.h"

#include <QClipboard>
#include <QMessageBox>

#include "ui_ActionWidget.h"

//============================================================================
ClipboardPasteWidget::ClipboardPasteWidget( QWidget* parent )
: ActionWidget( parent )
{
	setActionText( QObject::tr( "Paste from clipboard" ) );
	setActionIcon( eMyIconEditPaste );
	connect( this, SIGNAL(clicked()), this, SLOT(slotAction()) );
}

//============================================================================
void ClipboardPasteWidget::slotAction( void )
{
	QClipboard * clipboard = QApplication::clipboard();
    if( clipboard->text().isEmpty() )
    {
        QMessageBox::information( this, QObject::tr( "Clipboard Is Empty" ), QObject::tr( "Cannot paste empty clipboard" ), QMessageBox::Ok );
        return;
    }

    emit signalClipboardPaste( clipboard->text() );
}


