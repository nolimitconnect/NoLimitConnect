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

#include "GuiGroupieListSession.h"
#include "GuiUser.h"

//============================================================================
GuiGroupieListSession::GuiGroupieListSession( QWidget* parent )
    : QWidget( parent )
{
}

//============================================================================
GuiGroupieListSession::GuiGroupieListSession( GroupieId& hostedId, GuiGroupie* guiGroupie, QWidget* parent )
    : QWidget( parent )
    , m_GroupieId( hostedId )
    , m_GuiGroupie( guiGroupie )
{
}

//============================================================================
GuiGroupieListSession::GuiGroupieListSession( const GuiGroupieListSession &rhs )
    : QWidget()
    , m_GroupieId( rhs.m_GroupieId )
    , m_GuiGroupie( rhs.m_GuiGroupie )
    , m_SessionId( rhs.m_SessionId )
{
}

//============================================================================
GuiGroupieListSession& GuiGroupieListSession::operator =( const GuiGroupieListSession &rhs )
{
	if( this != &rhs )   
	{
        m_GroupieId              = rhs.m_GroupieId;
        m_SessionId				= rhs.m_SessionId;
        m_GuiGroupie             = rhs.m_GuiGroupie;
	}

	return *this;
}

//============================================================================
std::string GuiGroupieListSession::getGroupieUrl( bool ipv6 )
{
    if( m_GuiGroupie )
    {
        return m_GuiGroupie->getGroupieUrl( ipv6 );
    }

    return "";
}

//============================================================================
std::string GuiGroupieListSession::getGroupieTitle( void )
{
    if( m_GuiGroupie )
    {
        return m_GuiGroupie->getGroupieTitle();
    }

    return "";
}

//============================================================================
std::string GuiGroupieListSession::getGroupieDescription( void )
{
    if( m_GuiGroupie )
    {
        return m_GuiGroupie->getGroupieDescription();
    }

    return "";
}

//============================================================================
VxGUID GuiGroupieListSession::getHostThumbId( void )
{
    if( m_GuiGroupie && m_GuiGroupie->getUser() )
    {
        return m_GuiGroupie->getUser()->getHostThumbId( m_GroupieId.getHostType(), true );
    }

    VxGUID emptyThumbId;
    return emptyThumbId;
}

//============================================================================
void GuiGroupieListSession::updateUser( GuiUser* guiUser )
{
    if( m_GuiGroupie && m_GuiGroupie->getUser() != guiUser )
    {
        return m_GuiGroupie->setUser( guiUser );
    }
}
