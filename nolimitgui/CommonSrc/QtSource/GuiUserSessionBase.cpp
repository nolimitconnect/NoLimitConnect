//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "GuiUserSessionBase.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <PktLib/VxSearchDefs.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxFileInfo.h>

#include <QObject>

//============================================================================
GuiUserSessionBase::GuiUserSessionBase( QWidget* parent )
    : QWidget( parent )
{
}

//============================================================================
GuiUserSessionBase::GuiUserSessionBase( GuiUser* hostIdent, QWidget* parent )
    : QWidget( parent )
    , m_Userdent( hostIdent )
{
    setUserIdent( m_Userdent );
}

//============================================================================
GuiUserSessionBase::GuiUserSessionBase( EHostType hostType, VxGUID& sessionId, GuiUser* hostIdent, QWidget* parent )
    : QWidget( parent )
    , m_HostType( hostType )
    , m_Userdent( hostIdent )
    , m_OfferSessionId( sessionId )
{
    setUserIdent( m_Userdent );
}

//============================================================================
GuiUserSessionBase::GuiUserSessionBase( const GuiUserSessionBase &rhs )
    : QWidget()
    , m_HostType( rhs.m_HostType )
    , m_Userdent( rhs.m_Userdent )
    , m_OfferSessionId( rhs.m_OfferSessionId )
    , m_OnlineId( rhs.m_OnlineId )
{
}

//============================================================================
GuiUserSessionBase& GuiUserSessionBase::operator =( const GuiUserSessionBase &rhs )
{
	if( this != &rhs )   
	{
        m_HostType		        = rhs.m_HostType;
        m_OfferSessionId        = rhs.m_OfferSessionId;
        m_Userdent		        = rhs.m_Userdent;
        m_OnlineId		        = rhs.m_OnlineId;
	}

	return *this;
}

//============================================================================
void GuiUserSessionBase::setUserIdent( GuiUser* guiUser )
{ 
    m_Userdent = guiUser;
    if( m_Userdent )
    {
        m_OnlineId = m_Userdent->getMyOnlineId();
    }
}
