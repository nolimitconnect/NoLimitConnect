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

#include "GuiHostedListSession.h"
#include "GuiUser.h"

//============================================================================
GuiHostedListSession::GuiHostedListSession( QWidget* parent )
    : QWidget( parent )
{
}

//============================================================================
GuiHostedListSession::GuiHostedListSession( HostedId& hostedId, GuiHosted* guiHosted, QWidget* parent )
    : QWidget( parent )
    , m_GuiHosted( guiHosted )
    , m_HostedId( hostedId )
{
}

//============================================================================
GuiHostedListSession::GuiHostedListSession( const GuiHostedListSession &rhs )
    : QWidget()
    , m_GuiHosted( rhs.m_GuiHosted )
    , m_HostedId( rhs.m_HostedId )
    , m_SessionId( rhs.m_SessionId )
{
}

//============================================================================
GuiHostedListSession& GuiHostedListSession::operator =( const GuiHostedListSession &rhs )
{
	if( this != &rhs )   
	{
        m_GuiHosted             = rhs.m_GuiHosted;
        m_HostedId              = rhs.m_HostedId;
        m_SessionId				= rhs.m_SessionId;
	}

	return *this;
}

//============================================================================
std::string GuiHostedListSession::getHostUrl( void )
{
    if( m_GuiHosted )
    {
        return m_GuiHosted->getHostInviteUrl();
    }

    return "";
}

//============================================================================
std::string GuiHostedListSession::getHostTitle( void )
{
    if( m_GuiHosted )
    {
        return m_GuiHosted->getHostTitle();
    }

    return "";
}

//============================================================================
std::string GuiHostedListSession::getHostDescription( void )
{
    if( m_GuiHosted )
    {
        return m_GuiHosted->getHostDescription();
    }

    return "";
}

//============================================================================
VxGUID& GuiHostedListSession::getSessionId( void )
{
    if( !m_SessionId.isVxGUIDValid() )
    {
        m_SessionId.initializeWithNewVxGUID();
    }

    return m_SessionId; 
}

//============================================================================
VxGUID GuiHostedListSession::getHostThumbId( void )
{
    if( m_GuiHosted && m_GuiHosted->getUser() )
    {
        return m_GuiHosted->getUser()->getHostThumbId( m_HostedId.getHostType(), true );
    }
    else if( m_GuiHosted )
    {
        return m_GuiHosted->getThumbId();
    }

    VxGUID emptyThumbId;
    return emptyThumbId;
}
