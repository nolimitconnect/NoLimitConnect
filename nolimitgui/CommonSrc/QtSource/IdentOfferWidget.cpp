//============================================================================
// Copyright (C) 2013 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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