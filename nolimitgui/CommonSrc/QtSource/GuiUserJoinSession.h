#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include "GuiUser.h"

#include <PktLib/GroupieId.h>

#include <QWidget>

class P2PEngine;
class VxGUID;
class GuiUserJoin;

class GuiUserJoinSession : public QWidget
{
public:
	GuiUserJoinSession( QWidget* parent = nullptr );
    GuiUserJoinSession( GuiUserJoin* guiUserJoin, QWidget* parent = nullptr );
	GuiUserJoinSession(	VxGUID& sessionId, GuiUserJoin* guiUserJoin, QWidget* parent = nullptr );
	GuiUserJoinSession( const GuiUserJoinSession &rhs );

	GuiUserJoinSession&			operator =( const GuiUserJoinSession &rhs );

    GuiUser*                    getGuiUser( void );

    void				        setUserJoin( GuiUserJoin* guiUserJoin );
    GuiUserJoin*				getUserJoin( void )                         { return m_GuiUserJoin; }

    void						setGroupieId( GroupieId& groupieId )        { m_GroupieId = groupieId; }
    GroupieId&                  getGroupieId( void )                        { return m_GroupieId; }
    EHostType                   getHostType( void );

    void						setSessionId( VxGUID& sessionId )           { m_SessionId = sessionId; }
    VxGUID&                     getSessionId( void )                        { return m_SessionId; }

    std::string                 getOnlineName( void )                       { return getGuiUser() ? getGuiUser()->getOnlineName() : ""; }

    void                        updateUser( GuiUser* guiUser );

protected:
	//=== vars ===//
    GroupieId                   m_GroupieId;
    GuiUserJoin*                m_GuiUserJoin;
    VxGUID					    m_SessionId;
};

