//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ListWidgetBase.h"
#include "AppletPeerChangeFriendship.h"
#include "AppCommon.h"
#include "GuiHelpers.h"

//============================================================================
ListWidgetBase::ListWidgetBase( QWidget* parent )
    : QListWidget( parent )
    , m_MyApp( GetAppInstance() )
    , m_MyIcons( m_MyApp.getMyIcons() )
    , m_UserMgr( m_MyApp.getUserMgr() )
    , m_Engine( m_MyApp.getEngine() )
    , m_ThumbMgr( m_MyApp.getThumbMgr() )
{
    setFocusPolicy( Qt::NoFocus );
}

//============================================================================
void ListWidgetBase::launchChangeFriendship( GuiUser* guiUser )
{
    AppletPeerChangeFriendship* applet = dynamic_cast<AppletPeerChangeFriendship*>(m_MyApp.launchApplet( eAppletPeerChangeFriendship, GuiHelpers::findParentPage( dynamic_cast<QWidget*>(this->parent()) ) ));
    if( applet )
    {
        applet->setFriend( guiUser );
    }
}
