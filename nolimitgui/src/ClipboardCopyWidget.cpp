//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "ClipboardCopyWidget.h"

#include <QClipboard>
#include <QMessageBox>

#include "ui_ActionWidget.h"

//============================================================================
ClipboardCopyWidget::ClipboardCopyWidget( QWidget* parent )
: ActionWidget( parent )
{
	setActionText( QObject::tr( "Copy to clipboard" ) );
	setActionIcon( eMyIconEditCopy );
}

//============================================================================
bool ClipboardCopyWidget::copyToClipboard( QString text )
{
    if( text.isEmpty() )
    {
        QMessageBox::information( this, QObject::tr( "Text Is Empty" ), QObject::tr( "Cannot copy empty text to clipboard" ), QMessageBox::Ok );
        return false;
    }

    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setText( text );
    return true;
}
