//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FriendListEntryWidget.h"

#include "AppCommon.h"
#include "AppletMgr.h"
#include "AppletPopupMenu.h"
#include "GuiHelpers.h"
#include "GuiUser.h"

//============================================================================
FriendListEntryWidget::FriendListEntryWidget( QListWidget * parent, int type  )
: IdentWidget( parent )
, QListWidgetItem( parent, type )
{
}

//============================================================================
void FriendListEntryWidget::setUser( GuiUser* guiUser )
{
	QListWidgetItem::setData( Qt::UserRole + 2, QVariant( ( qulonglong )(guiUser) ) );
}

//============================================================================
GuiUser* FriendListEntryWidget::getUser( void )
{
	return (GuiUser*)QListWidgetItem::data( Qt::UserRole + 2 ).toULongLong();
}

//============================================================================
void FriendListEntryWidget::onIdentAvatarButtonClicked()
{
	emit listButtonClicked( this );
}

//============================================================================
void FriendListEntryWidget::onIdentOfferViewButtonClicked()
{
	emit listButtonClicked( this );
}

//============================================================================
void FriendListEntryWidget::onIdentOfferAcceptButtonClicked()
{
	emit listButtonClicked( this );
}

//============================================================================
void FriendListEntryWidget::onIdentOfferRejectButtonClicked()
{
	emit listButtonClicked( this );
}

//============================================================================
void FriendListEntryWidget::onIdentMenuButtonClicked()
{
	GuiUser* selectedFriend = getUser();
	if( selectedFriend )
	{
		AppletPopupMenu* applet = dynamic_cast<AppletPopupMenu*>(GetAppInstance().getAppletMgr().launchApplet( eAppletPopupMenu, GuiHelpers::getParentPageFrame( this ) ));
		if( applet )
		{
			applet->showPersonOfferMenu( selectedFriend );
		}
	}
}
