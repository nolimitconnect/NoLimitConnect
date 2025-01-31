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

#include <QLabel>

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
	//m_Engine.fromGuiMuteMicrophone( false );
	//m_Engine.fromGuiMuteSpeaker( false );
	wantActivityCallbacks( true );
}

//============================================================================
void AppletPeerBase::callbackUserAdded( GuiUser* guiUser )
{
    //LogMsg( LOG_DEBUG, "AppletPeerBase::%s", __func__ );
}

//============================================================================
void AppletPeerBase::callbackUserRemoved( VxGUID& onlineId )
{
    //LogMsg( LOG_DEBUG, "AppletPeerBase::%s", __func__ );
}

//============================================================================
void AppletPeerBase::callbackUserUpdated( GuiUser* guiUser )
{
    //LogMsg( LOG_DEBUG, "AppletPeerBase::%s", __func__ );
}

//============================================================================
void AppletPeerBase::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
    if(LogEnabled(eLogUsers))LogMsg( LOG_DEBUG, "AppletPeerBase::%s online ? %d user %s", __func__, isOnline, guiUser->getOnlineName().c_str() );
    if( !isOnline )
    {
        m_OfferSessionLogic.toGuiContactOffline( guiUser );
    }
}

//============================================================================
void AppletPeerBase::setupBaseWidgets( GuiUser*	guiUser, IdentWidget* friendIdentWidget, VxPushButton* permissionButton, QLabel* permissionLabel )
{
	if( friendIdentWidget )
	{
		friendIdentWidget->updateIdentity( guiUser );
	}

	if( permissionButton && guiUser )
	{
		EPluginAccess ePluginAccess = guiUser->getMyAccessPermissionFromHim( m_ePluginType );

		permissionButton->setIcon( getMyIcons().getPluginIcon( m_ePluginType, ePluginAccess ) );
		QString strAction = GuiParams::describePluginType( m_ePluginType );
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
        wantActivityCallbacks( true );
    }
}

//============================================================================
void AppletPeerBase::hideEvent( QHideEvent* ev )
{
    if( ePluginTypeInvalid == m_ePluginType )
    {
        m_MyApp.getPluginMgr().setPluginVisible( m_ePluginType, false );
        wantActivityCallbacks( false );
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
void AppletPeerBase::callbackToGuiRxedPluginOffer( std::shared_ptr<GuiOfferSession>& offer )
{
    m_OfferSessionLogic.callbackToGuiRxedPluginOffer( offer );
}; 

//============================================================================
void AppletPeerBase::callbackToGuiRxedOfferReply( std::shared_ptr<GuiOfferSession>& offerReply )
{
    m_OfferSessionLogic.callbackToGuiRxedOfferReply( offerReply );
}; 

//============================================================================
void AppletPeerBase::callbackToGuiPluginSessionEnded( std::shared_ptr<GuiOfferSession>& offer )
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
void AppletPeerBase::setOfferToIdentity( GuiUser* guiUser, bool inGroup )
{
	m_MyApp.offerToFriendPluginSession( guiUser, getPluginType(), inGroup, getParentPageFrame() );
}

//============================================================================
bool AppletPeerBase::setOfferSession( std::shared_ptr<GuiOfferSession> offerSession )   
{ 
	m_OfferSessionLogic.setGuiOfferSession( offerSession );
	onOfferWasSet();
	return true;
}

//============================================================================
void AppletPeerBase::slotEndSession( void )
{
	onActivityFinish();
	closeApplet();
}
