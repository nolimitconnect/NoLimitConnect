//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxPlainTextEdit.h"
#include "AppCommon.h"
#include "VxAppTheme.h"

#include <QMenu>

//============================================================================
VxPlainTextEdit::VxPlainTextEdit( QWidget* parent )
: QPlainTextEdit( parent )
{
}

//============================================================================
VxPlainTextEdit::VxPlainTextEdit( const QString &text, QWidget* parent )
    : QPlainTextEdit( text, parent )
{
}

//============================================================================
void VxPlainTextEdit::contextMenuEvent( QContextMenuEvent *event )
{
    QMenu *menu = createStandardContextMenu();
    GetAppInstance().getAppTheme().applyTheme( menu );
    menu->addAction( tr( "My Menu Item" ) );
    menu->exec( event->globalPos() );
    delete menu;
}
