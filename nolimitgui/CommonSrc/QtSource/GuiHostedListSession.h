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

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiHosted.h"

class GuiUser;

class GuiHostedListSession : public QWidget
{
public:
	GuiHostedListSession( QWidget* parent = nullptr );
	GuiHostedListSession( HostedId& hostedId, GuiHosted* guiHosted, QWidget* parent = nullptr );
	GuiHostedListSession( const GuiHostedListSession &rhs );

	GuiHostedListSession&		operator =( const GuiHostedListSession &rhs );

   
    void                        setGuiHosted( GuiHosted* guiHosted )        { m_GuiHosted = guiHosted; if( guiHosted->getUser() ) setGuiUser( guiHosted->getUser() ); }
    GuiHosted*                  getGuiHosted( void )                        { return m_GuiHosted; }

    HostedId&                   getHostedId( void )                         { return m_HostedId; }
    VxGUID&                     getHostOnlineId( void )                     { return m_HostedId.getHostOnlineId(); }
    EHostType                   getHostType( void )                         { return m_HostedId.getHostType(); }

    void                        setGuiUser( GuiUser* guiUser )              { if( m_GuiHosted && guiUser ) m_GuiHosted->setUser( guiUser ); }
    GuiUser*                    getGuiUser( void )                          { return m_GuiHosted ? m_GuiHosted->getUser() : nullptr; }

    std::string                 getHostUrl( bool ipv6 );
    std::string                 getHostTitle( void );
    std::string                 getHostDescription( void );

    void						setSessionId( VxGUID& sessionId )           { m_SessionId = sessionId; }
    VxGUID&                     getSessionId( void );

    void						setWidget( QWidget* widget )                { m_Widget = widget; }
    QWidget*                    getWidget( void )                           { return m_Widget; }

    VxGUID                      getHostThumbId( void );

    void                        updateUser( GuiUser* guiUser );

protected:
	//=== vars ===//
    GuiHosted*                  m_GuiHosted{ nullptr };
    HostedId                    m_HostedId;
    VxGUID					    m_SessionId;

    QWidget*					m_Widget{ nullptr };
};

