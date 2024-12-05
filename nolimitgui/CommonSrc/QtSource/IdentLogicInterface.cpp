//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "IdentLogicInterface.h"

#include "AppCommon.h"
#include "AppletPeerChangeFriendship.h"
#include "GuiGroupie.h"
#include "GuiHelpers.h"
#include "GuiUser.h"
#include "GuiHostJoin.h"
#include "GuiPushToTalkMgr.h"
#include "GuiRandConnectMgr.h"
#include "MyIcons.h"
#include "SoundMgr.h"
#include "VxPushButton.h"

#include <P2PEngine/P2PEngine.h>

//============================================================================
IdentLogicInterface::IdentLogicInterface( QWidget* parent )
	: QWidget( parent )
	, m_MyApp( GetAppInstance() )
{
	m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, true );
}

//============================================================================
IdentLogicInterface::~IdentLogicInterface()
{
	m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, false );
	wantPushToTalkCallbacks( false );
}

//============================================================================
void IdentLogicInterface::setupIdentLogic( void ) // call after derived class ui is called
{
	if( !m_IsSignalsConnected )
	{
		if( m_GuiUser && ShouldDebugUser( m_GuiUser->getOnlineName().c_str() ) )
		{
			LogModule( eLogUserEvent, LOG_VERBOSE, " IdentLogicInterface::updateIdentity %s %s my friendship %s his friendship %s ptr %p",
						m_GuiUser->getOnlineName().c_str(), m_GuiUser->getMyOnlineId().toOnlineIdString().c_str(),
						DescribeFriendState( m_GuiUser->getMyFriendshipToHim() ), DescribeFriendState( m_GuiUser->getHisFriendshipToMe() ), this );
		}

		m_IsSignalsConnected = true;
		if( getIdentOfferViewButton() )
		{
			getIdentOfferViewButton()->setVisible( false );
			connect( getIdentOfferViewButton(), SIGNAL(clicked()), this, SLOT(slotIdentOfferViewButtonClicked()) );
		}

		if( getIdentOfferAcceptButton() )
		{
			getIdentOfferAcceptButton()->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );
			getIdentOfferAcceptButton()->setIcon( eMyIconCheckMark );
			getIdentOfferAcceptButton()->setVisible( false );
			connect( getIdentOfferAcceptButton(), SIGNAL(clicked()), this, SLOT(slotIdentOfferAcceptButtonClicked()) );
		}

		if( getIdentOfferRejectButton() )
		{
			getIdentOfferRejectButton()->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
			getIdentOfferRejectButton()->setIcon( eMyIconRedX );
			getIdentOfferRejectButton()->setVisible( false );
			connect( getIdentOfferRejectButton(), SIGNAL(clicked()), this, SLOT(slotIdentOfferRejectButtonClicked()) );
		}

		if( getIdentPushToTalkButton() )
		{
			getIdentPushToTalkButton()->setVisible( false );
			connect( getIdentPushToTalkButton(), SIGNAL(clicked()), this, SLOT(slotIdentPushToTalkButtonClicked()) );
		}

		getIdentAvatarButton()->setIcon( eMyIconAvatarImage );
		getIdentFriendshipButton()->setIcon( eMyIconUnknown );

		connect( getIdentAvatarButton(), SIGNAL(clicked()), this, SLOT(slotIdentAvatarButtonClicked()) );
		connect( getIdentFriendshipButton(), SIGNAL(clicked()), this, SLOT(slotIdentFrienshipButtonClicked()) );
		connect( getIdentMenuButton(), SIGNAL(clicked()), this, SLOT(slotIdentMenuButtonClicked()) );
	}
	
	onIdentLogicIsSetup();
}

//============================================================================
void IdentLogicInterface::setupIdentLogic( enum EButtonSize buttonSize) // call after derived class ui is called.. also calls setIdentWidgetSize
{
	setIdentWidgetSize( buttonSize );
}

//============================================================================
void IdentLogicInterface::setIdentWidgetSize( enum EButtonSize buttonSize )
{
	QSize butSize = GuiParams::getButtonSize( buttonSize );
	if( getIdentLine3() )
	{
		if( buttonSize < eButtonSizeLarge )
		{
			// wont fit the third line
			getIdentLine3()->setVisible( false );
		}
		else
		{
			getIdentLine3()->setVisible( true );
		}
	}

	setFixedHeight( butSize.height() + 4 );
	getIdentAvatarButton()->setFixedSize( buttonSize );
	setIdentAvatarIcon( eMyIconAvatarImage );
	getIdentFriendshipButton()->setFixedSize( buttonSize );
	setIdentFriendshipIcon( eMyIconAnonymous );
	if( getIdentOfferViewButton() )
	{
		getIdentOfferViewButton()->setFixedSize( buttonSize );
	}

	if( getIdentOfferAcceptButton() )
	{
		getIdentOfferAcceptButton()->setFixedSize( buttonSize );
	}

	if( getIdentOfferRejectButton() )
	{
		getIdentOfferRejectButton()->setFixedSize( buttonSize );
	}
	
	if( getIdentPushToTalkButton() )
	{
		getIdentPushToTalkButton()->setFixedSize( buttonSize );
		getIdentPushToTalkButton()->setIcon( eMyIconPushToTalkOff );
	}

	getIdentMenuButton()->setFixedSize( buttonSize );
	setIdentMenuIcon( eMyIconMenu );
}

//============================================================================
void IdentLogicInterface::updateIdentity( VxGUID& onlineId )
{
	bool wasUpdated{ false };
	if( onlineId.isVxGUIDValid() )
	{
		GuiUser* guiUser = m_MyApp.getUserMgr().getUser( onlineId );
		if( guiUser )
		{
			updateIdentity( guiUser );
			wasUpdated = true;
		}
	}

	if( !wasUpdated )
	{
		clearIdentity();
	}
}

//============================================================================
void IdentLogicInterface::updateIdentity( GuiUser* guiUser, bool queryThumb )
{
	if( guiUser )
	{	
		if( guiUser->getNetIdent().isValidNetIdent() )
		{
			//if( ShouldDebugUser( guiUser->getOnlineName().c_str() ) )
			//{
			//	LogModule( eLogUserEvent, LOG_VERBOSE, " IdentLogicInterface::updateIdentity %s %s my friendship %s his friendship %s",
			//			   guiUser->getOnlineName().c_str(), guiUser->getMyOnlineId().toOnlineIdString().c_str(),
			//			   DescribeFriendState( guiUser->getMyFriendshipToHim() ), DescribeFriendState( guiUser->getHisFriendshipToMe() ) );
			//}

			if( !m_GuiUser  )
			{
				wantPushToTalkCallbacks( true );
			}
			else if( m_GuiUser != guiUser )
			{
				// clear the avatar icon in case there is none for this identity
				getIdentAvatarButton()->clear();
				getIdentAvatarButton()->setIcon( eMyIconAvatarImage );
			}

			m_GuiUser = guiUser;
			m_UserOnlineId = guiUser->getMyOnlineId();

			setupIdentLogic();

			bool isOnline = m_GuiUser->isOnline();
			bool isRelayed = m_GuiUser->isRelayed();

			getIdentLine1()->setText( m_GuiUser->getOnlineName().c_str() );
			getIdentLine2()->setText( m_GuiUser->getOnlineDescription().c_str() );
			getIdentFriendshipButton()->setIcon( m_MyApp.getMyIcons().getFriendshipIcon( m_GuiUser->getMyFriendshipToHim() ) );

			if( m_GuiUser->getMyFriendshipToHim() == eFriendStateFriend && m_GuiUser->getHisFriendshipToMe() == eFriendStateFriend )
			{
				getIdentFriendshipButton()->setIcon( eMyIconFriendJoined );
			}

			getIdentFriendshipButton()->setRandAction( m_MyApp.getRandConnectMgr().getRandAction( m_GuiUser->getMyOnlineId() ) );

			if( getIdentPushToTalkButton() )
			{
				if( isOnline && m_GuiUser->isMyAccessAllowedFromHim( ePluginTypePushToTalk ) && m_GuiUser->isHisAccessAllowedFromMe( ePluginTypePushToTalk ) )
				{
					getIdentPushToTalkButton()->setVisible( true );
					getIdentPushToTalkButton()->setPushToTalkStatus( m_MyApp.getPushToTalkMgr().getPushToTalkStatus( m_UserOnlineId ) );
				}
				else
				{
					getIdentPushToTalkButton()->setVisible( false );
				}
			}

			if( getIdentLine3() )
			{
				QString truths = QObject::tr( "Truths: " );
				QString dares = QObject::tr( " Dares: " );
				getIdentLine3()->setText( QString( truths + "%1" + dares + "%2" ).arg( m_GuiUser->getTruthCount() ).arg( m_GuiUser->getDareCount() ) );
			}

			bool isMyself = m_GuiUser->getMyOnlineId() == m_MyApp.getMyOnlineId();
			if( isMyself )
			{
				LogMsg( LOG_DEBUG, "GuiUserListWidget::updateIdentity myself %s", guiUser->getOnlineName().c_str() );
			}

			if( queryThumb )
			{
				setIdentAvatarThumbnail( m_GuiUser->getAvatarThumbGuid() );
			}

			if( isMyself )
			{
				getIdentFriendshipButton()->setIcon( eMyIconAdministrator ); // eMyIconAdministrator );
				getIdentFriendshipButton()->setNotifyType( eNotifyOnline );

				getIdentFriendshipButton()->setNotifyDirectConnectEnabled( true );
				getIdentFriendshipButton()->setNotifyDirectConnectColor( m_MyApp.getAppTheme().getColor( eLayerNotifyOnlineColor ) );
			}
			else
			{
				ENotifyType notifyType = isOnline ? eNotifyOnline : eNotifyOffline;
				EThemeColorRole onlineIndicatorColor{ eLayerNotifyOfflineColor };
				if( isOnline )
				{
					onlineIndicatorColor = isRelayed ? eLayerNotifyRelayedColor : eLayerNotifyOnlineColor;
					if( isRelayed )
					{
						notifyType = eNotifyRelayed;
					}		
				}

				getIdentFriendshipButton()->setNotifyType( notifyType );

                getIdentFriendshipButton()->setNotifyDirectConnectEnabled( notifyType == eNotifyOnline );
                if( notifyType == eNotifyOnline )
				{
					getIdentFriendshipButton()->setNotifyDirectConnectColor( m_MyApp.getAppTheme().getColor( onlineIndicatorColor ) );
				}

				getIdentFriendshipButton()->setNotifyNlcFavoriteEnabled( m_MyApp.getFavoriteMgr().getIsFavorite( m_GuiUser->getMyOnlineId() ) );
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "IdentLogicInterface::updateIdentity invalid net ident" );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "IdentLogicInterface::updateIdentity null guiUser" );
	}
}

//============================================================================
void IdentLogicInterface::clearIdentity( void )
{
	m_GuiUser = nullptr;
	getIdentLine1()->setText( "" );
	getIdentLine2()->setText( "" );

	getIdentAvatarButton()->clear();
	setIdentAvatarIcon( eMyIconAvatarImage );

	setIdentFriendshipIcon( eMyIconAnonymous );
	getIdentFriendshipButton()->setNotifyType( eNotifyNone );
	getIdentFriendshipButton()->setNotifyDirectConnectEnabled( false );

	if( getIdentPushToTalkButton() )
	{
		getIdentPushToTalkButton()->setVisible( false );
	}
}

//============================================================================
void IdentLogicInterface::updateHosted( GuiHosted* guiHosted )
{
	if( guiHosted )
	{
		GuiUser* guiUser = guiHosted->getUser();
		if( guiUser )
		{
			m_GuiUser = guiUser;
			if( m_GuiUser )
			{
				updateIdentity( m_GuiUser, false );
			}

			VxGUID thumbId = m_GuiUser->getHostThumbId( guiHosted->getHostType(), true );
			if( thumbId.isVxGUIDValid() )
			{
				setIdentAvatarThumbnail( thumbId );
			}
		}

		if( !guiHosted->getHostTitle().empty() )
		{
			getIdentLine1()->setText( guiHosted->getHostTitle().c_str() );
		}
		else
		{
			LogMsg( LOG_ERROR, "IdentLogicInterface::updateHosted no host title" );
		}

		if( !guiHosted->getHostDescription().empty() )
		{
			getIdentLine2()->setText( guiHosted->getHostDescription().c_str() );
		}
		else
		{
			LogMsg( LOG_ERROR, "IdentLogicInterface::updateHosted no host description" );
		}
		

        getIdentFriendshipButton()->setNotifyNlcFavoriteEnabled( m_MyApp.getFavoriteMgr().getIsFavorite( guiHosted->getAdminId().getHostOnlineId() ) );
	}
	else
	{
		LogMsg( LOG_ERROR, "IdentLogicInterface::updateHosted null guiHosted" );
	}
}

//============================================================================
void IdentLogicInterface::updateIdentity( GuiHostJoin* hostIdent, bool queryThumb )
{
	m_GuiUser = hostIdent->getUser();
	updateIdentity( hostIdent->getUser(), queryThumb );
}

//============================================================================
void IdentLogicInterface::updateIdentity( VxNetIdent* netIdent, bool queryThumb )
{
	if( netIdent )
	{
		updateIdentity( m_MyApp.getUserMgr().getUser( netIdent->getMyOnlineId() ) );
	}
}

//============================================================================
void IdentLogicInterface::setIdentAvatarButtonVisible( bool visible )
{
	getIdentAvatarButton()->setVisible( visible );
}

//============================================================================
void IdentLogicInterface::setIdentFriendshipButtonVisible( bool visible )
{
	getIdentFriendshipButton()->setVisible( visible );
}

//============================================================================
void IdentLogicInterface::setIdentOfferViewButtonVisible( bool visible )
{
	getIdentOfferViewButton()->setVisible( visible );
}

//============================================================================
void IdentLogicInterface::setIdentOfferAcceptButtonVisible( bool visible )
{
	getIdentOfferAcceptButton()->setVisible( visible );
}

//============================================================================
void IdentLogicInterface::setIdentOfferRejectButtonVisible( bool visible )
{
	getIdentOfferRejectButton()->setVisible( visible );
}

//============================================================================
void IdentLogicInterface::setIdentMenuButtonVisible( bool visible )
{
	getIdentMenuButton()->setVisible( visible );
}

//============================================================================
void IdentLogicInterface::setIdentOnlineState( bool isOnline )
{
	getIdentFriendshipButton()->setNotifyType( isOnline ? eNotifyOnline : eNotifyOffline );
}

//============================================================================
void IdentLogicInterface::setIdentGroupState( bool isInGroup )
{
	getIdentFriendshipButton()->setNotifyInGroupEnabled( isInGroup );
}

//============================================================================
void IdentLogicInterface::setIdentDirectConnectState( bool canDirectConnect )
{
	getIdentFriendshipButton()->setNotifyDirectConnectEnabled( canDirectConnect );
}

//============================================================================
void IdentLogicInterface::setIdentAvatarThumbnail( VxGUID thumbId )
{
	if( thumbId.isVxGUIDValid() )
	{
		LogModule( eLogAssets,  LOG_VERBOSE, "Thumb %s for user %s", thumbId.toOnlineIdString().c_str(), m_GuiUser->getOnlineName().c_str() );

		QImage thumbImage;
		if( m_MyApp.getThumbMgr().getThumbImage( thumbId, thumbImage ) )
		{
			getIdentAvatarButton()->setIconOverrideImage( thumbImage );
		}
	}
}

//============================================================================
void IdentLogicInterface::setIdentAvatarIcon( EMyIcons myIcon )
{
	getIdentAvatarButton()->setIcon( myIcon );
}

//============================================================================
void IdentLogicInterface::setIdentFriendshipIcon( EMyIcons myIcon )
{
	getIdentFriendshipButton()->setIcon( myIcon );
}

//============================================================================
void IdentLogicInterface::setIdentOfferViewIcon( EMyIcons myIcon )
{
	if( getIdentOfferViewButton() )
	{
		getIdentOfferViewButton()->setIcon( myIcon );
	}
}

//============================================================================
void IdentLogicInterface::setIdentOfferAcceptIcon( EMyIcons myIcon )
{
	if( getIdentOfferAcceptButton() )
	{
		getIdentOfferAcceptButton()->setIcon( myIcon );
	}
}

//============================================================================
void IdentLogicInterface::setIdentOfferRejectIcon( EMyIcons myIcon )
{
	if( getIdentOfferRejectButton() )
	{
		getIdentOfferRejectButton()->setIcon( myIcon );
	}
}

//============================================================================
void IdentLogicInterface::setIdentMenuIcon( EMyIcons myIcon )
{
	getIdentMenuButton()->setIcon( myIcon );
}

//============================================================================
void IdentLogicInterface::slotIdentAvatarButtonClicked( void )
{
	LogModule( eLogUserEvent, LOG_VERBOSE, "slotIdentAvatarButtonClicked %s", m_MyApp.describeUser( m_GuiUser ).c_str() );
	emit signalIdentAvatarButtonClicked();
	onIdentAvatarButtonClicked();
}

//============================================================================
void IdentLogicInterface::slotIdentFrienshipButtonClicked( void )
{
	LogModule( eLogUserEvent, LOG_VERBOSE, "slotIdentFrienshipButtonClicked %s", m_MyApp.describeUser( m_GuiUser ).c_str() );
	emit signalIdentFriendshipButtonClicked();
	onIdentFriendshipButtonClicked();
}

//============================================================================
void IdentLogicInterface::slotIdentOfferViewButtonClicked( void )
{
	LogModule( eLogUserEvent, LOG_VERBOSE, "slotIdentOfferViewButtonClicked %s", m_MyApp.describeUser( m_GuiUser ).c_str() );
	emit signalIdentOfferViewButtonClicked();
	onIdentOfferViewButtonClicked();
}

//============================================================================
void IdentLogicInterface::slotIdentOfferAcceptButtonClicked( void )
{
	LogModule( eLogUserEvent, LOG_VERBOSE, "slotIdentOfferAcceptButtonClicked %s", m_MyApp.describeUser( m_GuiUser ).c_str() );
	emit signalIdentOfferAcceptButtonClicked();
	onIdentOfferAcceptButtonClicked();
}

//============================================================================
void IdentLogicInterface::slotIdentOfferRejectButtonClicked( void )
{
	LogModule( eLogUserEvent, LOG_VERBOSE, "slotIdentOfferRejectButtonClicked %s", m_MyApp.describeUser( m_GuiUser ).c_str() );
	emit signalIdentOfferRejectButtonClicked();
	onIdentOfferRejectButtonClicked();
}

//============================================================================
void IdentLogicInterface::slotIdentMenuButtonClicked( void )
{
	LogModule( eLogUserEvent, LOG_VERBOSE, "slotIdentMenuButtonClicked %s", m_MyApp.describeUser( m_GuiUser ).c_str() );
	emit signalIdentMenuButtonClicked();
	onIdentMenuButtonClicked();
}

//============================================================================
void IdentLogicInterface::onIdentFriendshipButtonClicked( void )
{
	if( m_GuiUser && !m_DisableFriendshipChange )
	{
		QWidget* parentPage = GuiHelpers::findParentPage( dynamic_cast<QWidget*>(parent()) );
		if( parentPage )
		{
			AppletPeerChangeFriendship* applet = dynamic_cast<AppletPeerChangeFriendship*>(m_MyApp.launchApplet( eAppletPeerChangeFriendship, parentPage ));
			if( applet )
			{
				applet->setFriend( m_GuiUser );
			}
		}
	}
}

//============================================================================
void IdentLogicInterface::slotIdentPushToTalkButtonClicked( void )
{
	LogModule( eLogUserEvent, LOG_VERBOSE, "slotIdentPushToTalkButtonClicked %s", m_MyApp.describeUser( m_GuiUser ).c_str() );

	toggleIdentPushToTalk();
	onIdentPushToTalkButtonClicked();
	emit signalIdentPushToTalkButtonClicked();
}

//============================================================================
void IdentLogicInterface::toggleIdentPushToTalk( void )
{
	if( m_GuiUser )
	{
        EPushToTalkStatus status = m_MyApp.getPushToTalkMgr().getPushToTalkStatus( m_UserOnlineId );
        if( status == ePushToTalkStatusTxEnabled || status == ePushToTalkStatusDuplexEnabled )
		{
			if( !m_MyApp.getFromGuiInterface().fromGuiPushToTalk( m_UserOnlineId, false ) )
			{
				m_MyApp.getSoundMgr().playSnd( eSndDefBusy );
			}
		}
		else
		{
			if( !m_MyApp.getFromGuiInterface().fromGuiPushToTalk( m_UserOnlineId, true ) )
			{
				m_MyApp.getSoundMgr().playSnd( eSndDefBusy );
			}
		}
	}
	else
	{
        LogMsg( LOG_VERBOSE, "toggleIdentPushToTalk null gui user");
	}
}

//============================================================================
void IdentLogicInterface::updateGroupie( GuiGroupie* guiGroupie )
{
    if( !guiGroupie )
    {
        return;
    }

    if( guiGroupie->getUser() )
    {
        // if user is available update with user info
        updateIdentity( guiGroupie->getUser() );
    }
    else
    {
        // there is just the info the host sent.. fill in what is available
        bool isOnline = true;
        getIdentLine1()->setText( guiGroupie->getGroupieTitle().c_str() );
        getIdentLine2()->setText( guiGroupie->getGroupieDescription().c_str() );
        getIdentFriendshipButton()->setIcon( m_MyApp.getMyIcons().getFriendshipIcon( eFriendStateGuest ) );

        if( getIdentPushToTalkButton() )
        {
            getIdentPushToTalkButton()->setVisible( false );
        }

        bool isMyself = guiGroupie->getUserOnlineId() == m_MyApp.getMyOnlineId();
        if( isMyself )
        {
            // should not happen but just in case
            getIdentFriendshipButton()->setIcon( eMyIconAdministrator ); // eMyIconAdministrator );
            getIdentFriendshipButton()->setNotifyType( eNotifyOnline );

            getIdentFriendshipButton()->setNotifyDirectConnectEnabled( true );
            getIdentFriendshipButton()->setNotifyDirectConnectColor( m_MyApp.getAppTheme().getColor( eLayerNotifyOnlineColor ) );
        }
        else
        {
            getIdentFriendshipButton()->setNotifyOnline( isOnline );

            getIdentFriendshipButton()->setNotifyDirectConnectEnabled( false );
        }
    }
}

//============================================================================
void IdentLogicInterface::updateOffer( GuiOfferSession* offerSession )
{
	m_OfferSession = offerSession;
	getIdentOfferViewButton()->setVisible( true );

	getIdentOfferViewButton()->setIcon( m_MyApp.getMyIcons().getOfferIcon( m_OfferSession->getOfferInfo(), m_OfferSession->getPluginType() ) );
	bool isActive = m_OfferSession->isAvailableAndActiveOffer();
	getIdentOfferAcceptButton()->setVisible( isActive );
	getIdentOfferRejectButton()->setVisible( isActive );

	if( m_OfferSession->hasMessages() )
	{
		getIdentOfferViewButton()->setEyeOverlayColor( m_MyApp.getAppTheme().getColor( eLayerNotifyOnlineColor ) );
		getIdentOfferViewButton()->setEyeOverlayEnabled( true );
	}
	else
	{
		getIdentOfferViewButton()->setEyeOverlayEnabled( false );
	}

	if( isActive )
	{
		if( m_OfferSession->getOfferMsg().empty() )
		{
			getIdentLine3()->setText( "" );
			getIdentLine3()->setVisible( false );
		}
		else
		{
			getIdentLine3()->setText( m_OfferSession->getOfferMsg().c_str() );
			getIdentLine3()->setVisible( true );
		}
	}
	else
	{
		getIdentLine3()->setVisible( true );
		getIdentLine3()->setText( m_OfferSession->getActiveDescription() );
	}
}

//============================================================================
void IdentLogicInterface::callbackUserUpdated( GuiUser* guiUser )
{
	if( guiUser == m_GuiUser )
	{
		updateIdentity( m_GuiUser );
	}
}

//============================================================================
void IdentLogicInterface::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
	if( guiUser == m_GuiUser )
	{
		updateIdentity( m_GuiUser );
	}
}

//============================================================================
void IdentLogicInterface::callbackPushToTalkStatus( VxGUID& onlineId, enum EPushToTalkStatus pushToTalkStatus )
{
	if( onlineId == m_UserOnlineId )
	{
		if( getIdentPushToTalkButton() )
		{
			getIdentPushToTalkButton()->setPushToTalkStatus( pushToTalkStatus );
		}
	}
}

//============================================================================
void IdentLogicInterface::callbackGuiRandConnect( VxGUID& onlineId, enum ERandAction randAction )
{
	if( onlineId == m_UserOnlineId )
	{
		if( getIdentPushToTalkButton() )
		{
			getIdentFriendshipButton()->setRandAction( randAction );
		}
	}
}

//============================================================================
void IdentLogicInterface::wantPushToTalkCallbacks( bool enable )
{
    if( enable != m_PushToTalkCallbacksRequested )
    {
        m_PushToTalkCallbacksRequested = enable;
        m_MyApp.getPushToTalkMgr().wantGuiPushToTalkCallbacks( this, enable );
    }
}
