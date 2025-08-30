#pragma once
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

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletFriendRequestUi;
}
QT_END_NAMESPACE

class AppletFriendRequest : public AppletPeerBase
{
	Q_OBJECT
public:
	AppletFriendRequest( AppCommon& app, QWidget*	parent = nullptr );
	virtual ~AppletFriendRequest() override = default;

	void						friendRequestSetup( VxGUID onlineId, bool isAccept );

protected slots:
	void						slotPaste( QString pasteText );
	void						slotCopy( void );

	void						slotSend( void );
	void						slotCancel( void );
	void						slotAccept( void );
	void						slotReject( void );

protected:

    void						toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg ) override;
	void				        callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg ) override;

	//=== vars ===//
	Ui::AppletFriendRequestUi&	ui;

	bool						m_IsAccept{ false };
	GuiUser*					m_GuiUser{ nullptr }; 
};
