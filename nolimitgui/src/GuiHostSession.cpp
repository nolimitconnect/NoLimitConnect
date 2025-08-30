//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiHostSession.h"
#include "GuiUser.h"

//============================================================================
GuiHostSession::GuiHostSession( QWidget* parent )
    : QWidget( parent )
{
}

//============================================================================
GuiHostSession::GuiHostSession( EHostType hostType, VxGUID& sessionId, GuiUser* hostIdent, PluginSetting& pluginSetting, QWidget* parent )
    : QWidget( parent )
    , m_HostType( hostType )
    , m_SessionId( sessionId )
    , m_OnlineId( hostIdent->getMyOnlineId() )
    , m_HostIdent( hostIdent )
    , m_PluginSetting( pluginSetting )
{
}

//============================================================================
GuiHostSession::GuiHostSession( const GuiHostSession &rhs )
    : QWidget()
    , m_HostType( rhs.m_HostType )
    , m_SessionId( rhs.m_SessionId )
    , m_OnlineId( rhs.m_OnlineId )
    , m_HostIdent( rhs.m_HostIdent )
    , m_PluginSetting( rhs.m_PluginSetting )
{
}

//============================================================================
GuiHostSession& GuiHostSession::operator =( const GuiHostSession &rhs )
{
	if( this != &rhs )   
	{
        m_HostType		        = rhs.m_HostType;
        m_SessionId				= rhs.m_SessionId;
        m_HostIdent		        = rhs.m_HostIdent;
        m_PluginSetting			= rhs.m_PluginSetting;
        m_OnlineId			    = rhs.m_OnlineId;
	}

	return *this;
}

//============================================================================
std::string GuiHostSession::getHostUrl( void )
{
    return m_HostIdent->getMyOnlineUrl();
}

//============================================================================
std::string GuiHostSession::getHostDescription( void )
{
    if( !m_PluginSetting.getDescription().empty() )
    {
        return m_PluginSetting.getDescription();
    }
    else
    {
        return getHostUrl();
    }
}

//============================================================================
VxGUID GuiHostSession::getHostThumbId( void )
{
    return m_PluginSetting.getThumnailId();
}
