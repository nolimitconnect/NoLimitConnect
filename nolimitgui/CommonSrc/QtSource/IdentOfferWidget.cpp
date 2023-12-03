//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "IdentOfferWidget.h"

#include "AppCommon.h"
#include "AppletPopupMenu.h"
#include "GuiHelpers.h"

//============================================================================
IdentOfferWidget::IdentOfferWidget( QWidget* parent )
: IdentWidget(parent)
, m_MyApp( GetAppInstance() )
{
}

//============================================================================
void IdentOfferWidget::onIdentMenuButtonClicked( void )
{
    if( getGuiUser() )
    {
        QWidget* parentFrame = GuiHelpers::getParentPageFrame( dynamic_cast<QWidget*>(this->parent()) );
        AppletPopupMenu* popupMenu = dynamic_cast<AppletPopupMenu*>(m_MyApp.launchApplet( eAppletPopupMenu, parentFrame ) );
        if( popupMenu )
        {
            popupMenu->showFriendMenu( getGuiUser() );
        }
    }
}