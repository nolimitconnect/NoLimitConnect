//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiGroupieListItem.h"
#include "GuiGroupieListSession.h"
#include "GuiParams.h"
#include "AppCommon.h"

#include <CoreLib/VxDebug.h>

//============================================================================
GuiGroupieListItem::GuiGroupieListItem( QWidget* parent )
: IdentWidget( parent )
{
}

//============================================================================
GuiGroupieListItem::~GuiGroupieListItem()
{
    GuiGroupieListSession * groupieSession = (GuiGroupieListSession *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
    if( groupieSession && !groupieSession->parent() )
    {
        delete groupieSession;
    }
}

//============================================================================
QSize GuiGroupieListItem::calculateSizeHint( void )
{
    return QSize( ( int )( GuiParams::getGuiScale() * 200 ), ( int )( GuiParams::getButtonSize( eButtonSizeMedium ).height() + 8 ) );
}

//============================================================================
void GuiGroupieListItem::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    updateWidgetFromInfo();
}

//============================================================================
void GuiGroupieListItem::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
    emit signalGuiGroupieListItemClicked( this );
}

//============================================================================
void GuiGroupieListItem::setHostSession( GuiGroupieListSession* groupieSession )
{
    QListWidgetItem::setData( Qt::UserRole + 1, QVariant((quint64)groupieSession) );
}

//============================================================================
GuiGroupieListSession * GuiGroupieListItem::getHostSession( void )
{
    return (GuiGroupieListSession *)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
void GuiGroupieListItem::onIdentAvatarButtonClicked()
{
    LogMsg( LOG_DEBUG, "GuiGroupieListItem::slotIconButtonClicked" );
	emit signalIconButtonClicked( this );
}

//============================================================================
void GuiGroupieListItem::onIdentMenuButtonClicked( void )
{
	emit signalMenuButtonClicked( this );
}

//============================================================================
void GuiGroupieListItem::updateWidgetFromInfo( void )
{
    GuiGroupieListSession* groupieSession = getHostSession();
    if( nullptr == groupieSession )
    {
        return;
    }

    updateGroupie( groupieSession->getGroupie() );
}

//============================================================================
void GuiGroupieListItem::updateUser( GuiUser* guiUser )
{
    updateWidgetFromInfo();
}
