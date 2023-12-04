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

#include "GuiUser.h"

#include <QWidget>

class P2PEngine;
class VxGUID;
class PluginSetting;

class GuiUserSessionBase : public QWidget
{
    Q_OBJECT
public:
	GuiUserSessionBase( QWidget* parent = nullptr );
    GuiUserSessionBase( GuiUser* hostIdent, QWidget* parent = nullptr );
	GuiUserSessionBase(	EHostType hostType, VxGUID& sessionId, GuiUser* hostIdent, QWidget* parent = nullptr );
	GuiUserSessionBase( const GuiUserSessionBase &rhs );
    virtual ~GuiUserSessionBase() {};

	GuiUserSessionBase&			operator =( const GuiUserSessionBase &rhs );

    void				        setUserIdent( GuiUser* guiUser );
    GuiUser*				    getUserIdent( void )                        { return m_Userdent; }

    EHostType                   getHostType( void )                         { return m_HostType; }

    void						setOfferSessionId( VxGUID& sessionId )      { m_OfferSessionId = sessionId; }
    VxGUID&					    getOfferSessionId( void )                   { return m_OfferSessionId; }

    VxGUID&					    getMyOnlineId( void )                       { return m_OnlineId; }
    std::string                 getOnlineName( void )                       { return m_Userdent ? m_Userdent->getOnlineName() : ""; }

protected:
	//=== vars ===//
    EHostType                   m_HostType{ eHostTypeUnknown };
    GuiUser*                    m_Userdent{ nullptr };
    VxGUID					    m_OfferSessionId;
    VxGUID                      m_OnlineId;
};

