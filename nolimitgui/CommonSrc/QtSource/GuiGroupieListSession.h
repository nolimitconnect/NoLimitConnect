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

#include "GuiGroupie.h"

#include <QWidget>

class GuiUser;

class GuiGroupieListSession : public QWidget
{
public:
	GuiGroupieListSession( QWidget* parent = nullptr );
	GuiGroupieListSession( GroupieId& hostedId, GuiGroupie* guiGroupie, QWidget* parent = nullptr );
	GuiGroupieListSession( const GuiGroupieListSession &rhs );

	GuiGroupieListSession&		operator =( const GuiGroupieListSession &rhs );

    GuiUser*                    getUserIdent( void )                        { return m_GuiGroupie ? m_GuiGroupie->getUser() : nullptr; }

    GroupieId&                  getGroupieId( void )                        { return m_GroupieId; }
    GuiGroupie*                 getGroupie( void )                          { return m_GuiGroupie; }
    VxGUID&					    getUserOnlineId( void )                     { return m_GroupieId.getUserOnlineId(); }
    VxGUID&                     getHostOnlineId( void )                     { return m_GroupieId.getHostOnlineId(); }
    EHostType                   getHostType( void )                         { return m_GroupieId.getHostType(); }

    std::string                 getGroupieUrl( void );
    std::string                 getGroupieTitle( void );
    std::string                 getGroupieDescription( void );

    void						setSessionId( VxGUID& sessionId )           { m_SessionId = sessionId; }
    VxGUID&					    getSessionId( void )                        { return m_SessionId; }

    void						setWidget( QWidget* widget )                { m_Widget = widget; }
    QWidget*                    getWidget( void )                           { return m_Widget; }

    VxGUID                      getHostThumbId( void );

    void                        updateUser( GuiUser* guiUser );

protected:
	//=== vars ===//
    GroupieId                   m_GroupieId;
    GuiGroupie*                 m_GuiGroupie{ nullptr };
	VxGUID					    m_SessionId;

    QWidget*					m_Widget{ nullptr };
};

