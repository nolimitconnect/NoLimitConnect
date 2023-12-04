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

#include <GuiInterface/IDefs.h>
#include <ptop_src/ptop_engine_src/PluginSettings/PluginSetting.h>

#include <CoreLib/VxGUID.h>

#include <QWidget>

class P2PEngine;
class VxNetIdent;
class GuiUser;

class GuiHostSession : public QWidget
{
public:
	GuiHostSession( QWidget* parent = nullptr );
	GuiHostSession(	EHostType hostType, VxGUID& sessionId, GuiUser* hostIdent, PluginSetting& pluginSetting, QWidget* parent = nullptr );
	GuiHostSession( const GuiHostSession &rhs );

	GuiHostSession&			    operator =( const GuiHostSession &rhs );

    GuiUser*                    getUserIdent( void )                        { return m_HostIdent; }
    EHostType                   getHostType( void )                         { return m_HostType; }
    std::string                 getHostUrl( bool ipv6 );

    VxGUID&					    getOnlineId( void )                         { return m_OnlineId; }

    PluginSetting&				getPluginSetting( void )                    { return m_PluginSetting; }
    void						setSessionId( VxGUID& sessionId )           { m_SessionId = sessionId; }
    VxGUID&					    getSessionId( void )                        { return m_SessionId; }

    void						setWidget( QWidget* widget )				{ m_Widget = widget; }
    QWidget*					getWidget( void )							{ return m_Widget; }

    std::string                 getHostDescription( void );
    VxGUID                      getHostThumbId( void );

protected:
	//=== vars ===//
    EHostType                   m_HostType{ eHostTypeUnknown };
	VxGUID					    m_SessionId;
    VxGUID					    m_OnlineId;
    GuiUser*			    	m_HostIdent{ nullptr };
    PluginSetting               m_PluginSetting;
    QWidget*					m_Widget{ nullptr };
};

