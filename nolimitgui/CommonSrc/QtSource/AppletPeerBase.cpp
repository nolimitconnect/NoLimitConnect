//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPeerBase.h"

#include "ActivityMessageBox.h"
#include "AppCommon.h"
#include "AppGlobals.h"

#include "IdentWidget.h"
#include "SoundMgr.h"

#include "GuiOfferSession.h"
#include "GuiParams.h"
#include "GuiPluginMgr.h"
#include "MyIcons.h"
#include "VidWidget.h"
#include "VxPushButton.h"

#include <P2PEngine/P2PEngine.h>


//============================================================================
AppletPeerBase::AppletPeerBase(	const char* objName, AppCommon& app, QWidget* parent )
: AppletHostBase( objName, app, parent )
, m_OfferSessionLogic( this, this, app )
{
	setupAppletPeerBase();
	m_MyApp.getOfferMgr().wantGuiOfferCallbacks( this, true );
}

//============================================================================
AppletPeerBase::~AppletPeerBase()
{
	m_MyApp.getOfferMgr().wantGuiOfferCallbacks( this, false );
}

//============================================================================
void AppletPeerBase::onActivityFinish( void )
{
	m_OfferSessionLogic.onStop();
}

//============================================================================
void AppletPeerBase::setupAppletPeerBase( void )
{
	m_Engine.fromGuiMuteMicrophone( false );
	m_Engine.fromGuiMuteSpeaker( false );
	m_MyApp.wantToGuiActivityCallbacks( this, true );
}

//============================================================================
void AppletPeerBase::callbackUserAdded( GuiUser* guiUser )
{
    LogMsg( LOG_DEBUG, "AppletPeerBase::callbackUserAdded" );
}

//============================================================================
void AppletPeerBase::callbackUserRemoved( VxGUID& onlineId )
{
    LogMsg( LOG_DEBUG, "AppletPeerBase::callbackUserRemoved" );
}

//============================================================================
void AppletPeerBase::callbackUserUpdated( GuiUser* guiUser )
{
    LogMsg( LOG_DEBUG, "AppletPeerBase::callbackUserUpdated" );
}

//============================================================================
void AppletPeerBase::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
    LogMsg( LOG_DEBUG, "AppletPeerBase::callbackOnlineStatusChange online ? %d user %s", isOnline, guiUser->getOnlineName().c_str() );
    if( !isOnline )
    {
        m_OfferSessionLogic.toGuiContactOffline( guiUser );
    }
}

//============================================================================
void AppletPeerBase::setupBaseWidgets( IdentWidget*	friendIdentWidget, VxPushButton* permissionButton, QLabel* permissionLabel )
{
	if( friendIdentWidget )
	{
		friendIdentWidget->updateIdentity( m_HisIdent );
	}

	if( permissionButton && m_HisIdent )
	{
		EPluginAccess ePluginAccess = m_HisIdent->getMyAccessPermissionFromHim( m_ePluginType );

		permissionButton->setIcon( getMyIcons().getPluginIcon( m_ePluginType, ePluginAccess ) );
		QString strAction = DescribePluginType( m_ePluginType );
		if( 0 != permissionLabel )
		{
			permissionLabel->setText( strAction );	
		}
	}
}

//============================================================================
void AppletPeerBase::showEvent( QShowEvent* ev )
{
	AppletBase::showEvent( ev );
    if( ePluginTypeInvalid == m_ePluginType )
    {
        m_MyApp.getPluginMgr().setPluginVisible( m_ePluginType, true );
        m_MyApp.wantToGuiActivityCallbacks( this, true );
    }
}

//============================================================================
void AppletPeerBase::hideEvent( QHideEvent* ev )
{
    if( ePluginTypeInvalid == m_ePluginType )
    {
        m_MyApp.getPluginMgr().setPluginVisible( m_ePluginType, false );
        m_MyApp.wantToGuiActivityCallbacks( this, false );
    }

    AppletBase::hideEvent( ev );
}

//============================================================================   
void AppletPeerBase::closeEvent( QCloseEvent * ev )
{
	m_OfferSessionLogic.onStop();
    AppletBase::closeEvent( ev );
}

//============================================================================
bool AppletPeerBase::fromGuiSetGameValueVar( int32_t s32VarId, int32_t s32VarValue )
{
	bool bResult = m_FromGui.fromGuiSetGameValueVar(	m_ePluginType, 
														m_HisIdent->getMyOnlineId(),
														s32VarId,
														s32VarValue );
	if( false == bResult )
	{
		m_OfferSessionLogic.handleUserWentOffline();
	}

	return bResult;
}

//============================================================================
bool AppletPeerBase::fromGuiSetGameActionVar( int32_t s32VarId, int32_t s32VarValue )
{
	bool bResult = m_FromGui.fromGuiSetGameActionVar(	m_ePluginType, 
														m_HisIdent->getMyOnlineId(),
														s32VarId,
														s32VarValue );
	if( false == bResult )
	{
		m_OfferSessionLogic.handleUserWentOffline();
	}

	return bResult;
}

//============================================================================
void AppletPeerBase::callbackToGuiRxedPluginOffer( GuiOfferSession* offer )
{
	//m_OfferSessionLogic.toGuiRxedPluginOffer( offer );
}; 

//============================================================================
void AppletPeerBase::callbackToGuiRxedOfferReply( GuiOfferSession* offerReply )
{
	//m_OfferSessionLogic.toGuiRxedOfferReply( offerReply );
}; 

//============================================================================
void AppletPeerBase::callbackToGuiPluginSessionEnded( GuiOfferSession* offer )
{
	//handleSessionEnded( offerResponse, this );
}; 

//============================================================================
void AppletPeerBase::callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )
{
	if( m_VidCamWidget && m_VidCamWidget->isVisible() )
	{
		m_VidCamWidget->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
	}
}

//============================================================================
void AppletPeerBase::toGuiSetGameValueVar(	EPluginType     pluginType, 
											VxGUID&		    onlineId, 
											int32_t			s32VarId, 
											int32_t			s32VarValue )
{
	if( ( pluginType == m_ePluginType )
		&& ( onlineId == m_HisIdent->getMyOnlineId() ) )
	{
		//emit signalToGuiSetGameValueVar( s32VarId, s32VarValue );
	}
}

//============================================================================
void AppletPeerBase::toGuiSetGameActionVar(	EPluginType     pluginType, 
											VxGUID&		    onlineId, 
											int32_t			s32VarId, 
											int32_t			s32VarValue )
{
	if( ( pluginType == m_ePluginType )
		&& ( onlineId == m_HisIdent->getMyOnlineId() ) )
	{
		//emit signalToGuiSetGameActionVar( s32VarId, s32VarValue );
	}
}

//============================================================================
bool AppletPeerBase::offerSession( GuiUser* guiUser, GuiOfferSession* existingOffer )
{


	return false;
}
