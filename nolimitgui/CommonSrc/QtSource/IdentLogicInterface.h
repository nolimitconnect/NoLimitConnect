#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiParams.h"
#include "GuiUserUpdateCallback.h"
#include "MyIconsDefs.h"

#include <QWidget>

class AppCommon;
class VxGUID;
class VxPushButton;
class VxNetIdent;
class QLabel;
class GuiUser;
class GuiHostJoin;
class GuiHosted;
class GuiUserJoin;
class GuiGroupie;
class GuiOfferSession;

class IdentLogicInterface : public QWidget, public GuiUserUpdateCallback
{
	Q_OBJECT
public:
	IdentLogicInterface() = delete;
	IdentLogicInterface( QWidget* parent );
	virtual ~IdentLogicInterface();

	virtual void				setupIdentLogic( void ); // = ePluginTypeInvalid ); // call after derived class ui is called
	virtual void				setupIdentLogic( enum EButtonSize buttonSize ); // call after derived class ui is called.. also calls setIdentWidgetSize
	virtual void				onIdentLogicIsSetup( void ) {};

	virtual void				setIdentWidgetSize( enum EButtonSize buttonSize );

	virtual void				setPluginType( EPluginType pluginType )		{ m_PluginType = pluginType;  };
	EPluginType					getPluginType( void )						{ return m_PluginType; };

	virtual void				setHostType( EHostType hostType )			{ m_HostType = hostType; };
	EHostType					getHostType( void )							{ return m_HostType; };

	virtual VxPushButton*		getIdentAvatarButton( void ) = 0;
	virtual VxPushButton*		getIdentFriendshipButton( void ) = 0;
	virtual VxPushButton*		getIdentPushToTalkButton( void )			{ return nullptr; }
	virtual VxPushButton*		getIdentOfferViewButton( void )				{ return nullptr; }
	virtual VxPushButton*		getIdentOfferAcceptButton( void )			{ return nullptr; }
	virtual VxPushButton*		getIdentOfferRejectButton( void )			{ return nullptr; }
	virtual VxPushButton*		getIdentMenuButton( void ) = 0;

	virtual QLabel*				getIdentLine1( void ) = 0;
	virtual QLabel*				getIdentLine2( void ) = 0;
	virtual QLabel*				getIdentLine3( void )						{ return nullptr; }

	virtual void				clearIdentity( void );

	virtual void				updateIdentity( GuiUser* guiUser, bool queryThumb = true );
	virtual void				updateIdentity( GuiHostJoin* hostIdent, bool queryThumb = true );
	virtual void				updateIdentity( VxNetIdent* netIdent, bool queryThumb = true );
	virtual void				updateHosted( GuiHosted* guiHosted );
    virtual void				updateGroupie( GuiGroupie* guiGroupie );
	virtual void				updateOffer( GuiOfferSession* offerSession );


	virtual GuiUser*			getGuiUser( void )							{ return m_GuiUser; }

	void						setDisableFriendshipChange( bool disable )	{ m_DisableFriendshipChange = disable; }

	void						setIdentOnlineState( bool isOnline );
	void						setIdentGroupState( bool isInGroup );
	void						setIdentDirectConnectState( bool canDirectConnect );

	void						setIdentAvatarButtonVisible( bool visible );
	void						setIdentFriendshipButtonVisible( bool visible );
	void						setIdentOfferViewButtonVisible( bool visible );
	void						setIdentOfferAcceptButtonVisible( bool visible );
	void						setIdentOfferRejectButtonVisible( bool visible );
	void						setIdentMenuButtonVisible( bool visible );

	void						setIdentAvatarThumbnail( VxGUID thumbId );
	void						setIdentAvatarIcon( EMyIcons myIcon );
	void						setIdentFriendshipIcon( EMyIcons myIcon );
	void						setIdentOfferViewIcon( EMyIcons myIcon );
	void						setIdentOfferAcceptIcon( EMyIcons myIcon );
	void						setIdentOfferRejectIcon( EMyIcons myIcon );
	void						setIdentMenuIcon( EMyIcons myIcon );

	virtual void				onIdentAvatarButtonClicked( void )		{};
	virtual void				onIdentFriendshipButtonClicked( void );
	virtual void				onIdentOfferViewButtonClicked( void )	{};
	virtual void				onIdentOfferAcceptButtonClicked( void ) {};
	virtual void				onIdentOfferRejectButtonClicked( void ) {};
	virtual void				onIdentMenuButtonClicked( void )		{};

	virtual void				onIdentPushToTalkButtonClicked( void )  {};

	virtual void 				toggleIdentPushToTalk( void );

signals:
	void						signalIdentAvatarButtonClicked( void );
	void						signalIdentFriendshipButtonClicked( void );
	void						signalIdentOfferViewButtonClicked( void );
	void						signalIdentOfferAcceptButtonClicked( void );
	void						signalIdentOfferRejectButtonClicked( void );
	void						signalIdentMenuButtonClicked( void );
	void						signalIdentPushToTalkButtonClicked( void );

protected slots:
	void						slotIdentAvatarButtonClicked( void );
	void						slotIdentFrienshipButtonClicked( void );
	void						slotIdentOfferViewButtonClicked( void );
	void						slotIdentOfferAcceptButtonClicked( void );
	void						slotIdentOfferRejectButtonClicked( void );
	void						slotIdentMenuButtonClicked( void );

	void						slotIdentPushToTalkButtonClicked( void );

protected:
	virtual void				callbackUserUpdated( GuiUser* guiUser ) override;
	virtual void				callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) override;

	AppCommon&					m_MyApp;
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	EHostType					m_HostType{ eHostTypeUnknown };
	GuiUser*					m_GuiUser{ nullptr };
	GuiOfferSession*			m_OfferSession{ nullptr };
	bool						m_IsSignalsConnected{ false };
	bool						m_DisableFriendshipChange{ false };
};
