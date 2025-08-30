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

#include "GuiHostJoinSession.h"
#include "GuiHostJoin.h"
#include "GuiUser.h"

//============================================================================
GuiHostJoinSession::GuiHostJoinSession( QWidget* parentWidget )
    : QWidget( parentWidget )
{
}

//============================================================================
GuiHostJoinSession::GuiHostJoinSession( GuiHostJoin* guiHostJoin, QWidget* parentWidget )
    : QWidget( parentWidget )
    , m_GroupieId( guiHostJoin->getGroupieId() )
    , m_GuiHostJoin( guiHostJoin )
{
}

//============================================================================
GuiHostJoinSession::GuiHostJoinSession( const GuiHostJoinSession &rhs )
    : QWidget()
    , m_GroupieId( rhs.m_GroupieId )
    , m_GuiHostJoin( rhs.m_GuiHostJoin )
    , m_SessionId( rhs.m_SessionId )
{
}

//============================================================================
GuiHostJoinSession& GuiHostJoinSession::operator =( const GuiHostJoinSession &rhs )
{
	if( this != &rhs )   
	{
        m_GroupieId             = rhs.m_GroupieId;
        m_GuiHostJoin           = rhs.m_GuiHostJoin;
        m_SessionId             = rhs.m_SessionId;
	}

	return *this;
}

//============================================================================
std::string GuiHostJoinSession::getGroupieUrl( void )
{
    return m_GuiHostJoin->getUser()->getMyOnlineUrl();
}

//============================================================================
std::string GuiHostJoinSession::getHostUrl( void )
{
    return m_GuiHostJoin->getUser()->getMyOnlineUrl();
}

//============================================================================
std::string GuiHostJoinSession::getHostDescription( void )
{
    if( !m_GuiHostJoin->getUser()->getOnlineDescription().empty() )
    {
        return m_GuiHostJoin->getUser()->getOnlineDescription();
    }
    else
    {
        return getHostUrl();
    }
}

//============================================================================
VxGUID GuiHostJoinSession::getHostThumbId( void )
{
    return m_GuiHostJoin->getUser()->getAvatarThumbId();
}

//============================================================================
void GuiHostJoinSession::setHostJoin( GuiHostJoin* guiHostJoin )
{
    m_GuiHostJoin = guiHostJoin;
    if( m_GuiHostJoin )
    {
        m_GroupieId = guiHostJoin->getGroupieId();
    }
    else
    {
        m_GroupieId.clear();
    }
}

//============================================================================
GuiUser* GuiHostJoinSession::getGuiUser( void )
{ 
    return m_GuiHostJoin ? m_GuiHostJoin->getUser() : nullptr; 
}

//============================================================================
EHostType GuiHostJoinSession::getHostType( void )
{
    return m_GroupieId.getHostType();
}
