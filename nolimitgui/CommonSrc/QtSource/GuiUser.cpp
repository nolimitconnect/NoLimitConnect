//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiUser.h"

#include "AppCommon.h"
#include "GuiParams.h"
#include "GuiUserMgr.h"

#include <CoreLib/VxParse.h>
#include <CoreLib/VxTimeUtil.h>

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
    return m_NetIdent.isValidNetIdent() && isOnline() && !isRelayed();
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

//============================================================================
std::string GuiUser::dumpUserInfo( void )
{
    std::string userInfo;
    VxNetIdent& netIdent = getNetIdent();
    userInfo = "Online Id: " + getMyOnlineId().toOnlineIdString();
    StdStringAppendText( userInfo, "\nOnline Name: %s \n", getOnlineName().c_str() );
    StdStringAppendText( userInfo, "Description: %s \n", getOnlineDescription().c_str() );
    StdStringAppendText( userInfo, "My Friendship to him/her: %s \n", DescribeFriendState(getMyFriendshipToHim()) );
    StdStringAppendText( userInfo, "His/Her Friendship to me: %s \n", DescribeFriendState(getHisFriendshipToMe()) );
    StdStringAppendText( userInfo, "Last Connected: %s \n", VxTimeUtil::formatTimeStampIntoDateAndTimeWithTextMonths( netIdent.getLastSessionTimeMs() ).c_str() );


    StdStringAppendText( userInfo, "IP URL: %s \n", netIdent.getMyOnlineUrl().c_str() );

    LogMsg( LOG_INFO, "GuiUser::dumpUserInfo %s ", userInfo.c_str() );

    return userInfo;
}
