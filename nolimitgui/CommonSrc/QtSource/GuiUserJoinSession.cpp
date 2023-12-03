//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiUserJoinSession.h"
#include "GuiUserJoin.h"

//============================================================================
GuiUserJoinSession::GuiUserJoinSession( QWidget* parent )
    : QWidget( parent )
{
}

//============================================================================
GuiUserJoinSession::GuiUserJoinSession( GuiUserJoin* guiUserJoin, QWidget* parent )
    : QWidget( parent )
    , m_GroupieId( guiUserJoin->getGroupieId() )
    , m_GuiUserJoin( guiUserJoin )
{
}

//============================================================================
GuiUserJoinSession::GuiUserJoinSession( VxGUID& sessionId, GuiUserJoin* guiUserJoin, QWidget* parent )
    : QWidget( parent )
    , m_GroupieId( guiUserJoin->getGroupieId() )
    , m_GuiUserJoin( guiUserJoin )
    , m_SessionId( sessionId )
{
}

//============================================================================
GuiUserJoinSession::GuiUserJoinSession( const GuiUserJoinSession &rhs )
    : QWidget()
    , m_GroupieId( rhs.m_GroupieId )
    , m_GuiUserJoin( rhs.m_GuiUserJoin )
    , m_SessionId( rhs.m_SessionId )
{
}

//============================================================================
GuiUserJoinSession& GuiUserJoinSession::operator =( const GuiUserJoinSession &rhs )
{
	if( this != &rhs )   
	{
        m_GroupieId             = rhs.m_GroupieId;
        m_SessionId				= rhs.m_SessionId;
        m_GuiUserJoin           = rhs.m_GuiUserJoin;
	}

	return *this;
}

//============================================================================
void GuiUserJoinSession::setUserJoin( GuiUserJoin* guiUserJoin )
{ 
    m_GuiUserJoin = guiUserJoin;
    if( m_GuiUserJoin )
    {
        m_GroupieId = guiUserJoin->getGroupieId();
    }
    else
    {
        m_GroupieId.clear();
    }
}

//============================================================================
GuiUser* GuiUserJoinSession::getGuiUser( void )
{
    return m_GuiUserJoin ? m_GuiUserJoin->getUser() : nullptr;
}

//============================================================================
EHostType GuiUserJoinSession::getHostType( void )
{ 
    return m_GroupieId.getHostType(); 
}

//============================================================================
void GuiUserJoinSession::updateUser( GuiUser* guiUser )
{
    if( m_GuiUserJoin && m_GuiUserJoin->getUser() != guiUser )
    {
        return m_GuiUserJoin->setUser( guiUser );
    }
}
