//============================================================================
// Copyright (C) 2021 Brett R. Jones
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

#include "AppCommon.h"
#include "GuiUser.h"
#include "GuiUserMgr.h"
#include "GuiParams.h"

//============================================================================
GuiUser::GuiUser( AppCommon& app )
    : GuiUserBase( app )
    , m_UserMgr( m_MyApp.getUserMgr() )
{
}

//============================================================================
GuiUser::GuiUser( AppCommon& app, VxNetIdent* netIdent, VxGUID& sessionId )
    : GuiUserBase( app, netIdent, sessionId )
    , m_UserMgr( m_MyApp.getUserMgr() )
{
}

//============================================================================
GuiUser::GuiUser( const GuiUser& rhs )
    : GuiUserBase( rhs )
    , m_UserMgr( rhs.m_UserMgr )
{
}

//============================================================================
bool GuiUser::canDirectConnectToUser( void )
{
    return m_NetIdent.isValidNetIdent() && (isOnline() || isNearby()) && !isRelayed();
}

//============================================================================
VxGUID GuiUser::getAvatarThumbGuid( void ) 
{ 
    if( !m_NetIdent.isValidNetIdent() )
    {
        VxGUID guid;
        return guid;
    }

    return m_NetIdent.getAvatarThumbGuid();
}

//============================================================================
bool GuiUser::onlineNameMatch( QString& searchText )
{
    bool isMatch{ false };
    if( searchText.isEmpty() )
    {
        return true;
    }
    else
    {
        QString onlineName( m_NetIdent.getOnlineName() );
        if( searchText.length() <= onlineName.length() )
        {
            isMatch = onlineName.startsWith( searchText, Qt::CaseInsensitive );
        }
    }

    return isMatch;
}

//============================================================================
bool GuiUser::compareLessThan( GuiUser* guiOther )
{
    bool isLessThan{ false };
    if( guiOther && guiOther->getMyOnlineId() != getMyOnlineId() )
    {
        bool iAmFavorite = getIsFavorite();
        bool heIsFavorite = guiOther->getIsFavorite();
        if( iAmFavorite == heIsFavorite )
        {
            QString myName( m_NetIdent.getOnlineName() );
            QString hisName( guiOther->m_NetIdent.getOnlineName() );
            isLessThan = myName < hisName;
        }
        else if( !iAmFavorite && heIsFavorite )
        {
            isLessThan = true;
        }
    }

    return isLessThan;
}

//============================================================================
bool GuiUser::getIsFavorite( void )
{
    return m_MyApp.getFavoriteMgr().getIsFavorite( getMyOnlineId() );
}