#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include "ui_AppletJoinHostList.h"

#include "GuiUserUpdateCallback.h"
#include "GuiHostedListCallback.h"
#include "GuiGroupieListCallback.h"
#include "GuiHostJoinCallback.h"
#include "GuiUserJoinCallback.h"

#include <CoreLib/VxPtopUrl.h>

class GuiHostedListItem;
class GuiHostedListSession;

class AppletJoinBase : public AppletBase, public GuiUserUpdateCallback, public GuiHostedListCallback, public GuiGroupieListCallback, public GuiHostJoinCallback, public GuiUserJoinCallback
{
	Q_OBJECT
public:
    AppletJoinBase( const char*name, AppCommon& app, QWidget* parent );
	virtual ~AppletJoinBase() override;

    void                        setHostType( EHostType hostType );
	EHostType                   getHostType( void )										{ return m_HostType; }

	void                        setNetworkHostOnlineId( VxGUID& networkHostOnlineId )	{ m_NetworkHostOnlineId = networkHostOnlineId; }
	VxGUID&                     getNetworkHostOnlineId( void )							{ return m_NetworkHostOnlineId; }

	VxPtopUrl&					getNetworkHostPtopUrl( void )							{ return m_NetHostPtopUrl; }
	bool						isNetworkHostUrlValid( void )							{ return m_NetHostPtopUrl.isValid(); }

    virtual void                setStatusMsg( QString statusMsg );
    virtual void                setListLabel( QString labelText );

    virtual void				callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )  override;

    virtual void				callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override {};
    virtual void				callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId ) override {};
    virtual void				callbackUserAdded( GuiUser* guiUser ) override;
    virtual void				callbackUserUpdated( GuiUser* guiUser ) override;
    virtual void				callbackUserRemoved( VxGUID& onlineId ) override {};

    virtual void				callbackGuiHostedListSearchResult( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId ) override;
    virtual void				callbackGuiHostedListSearchComplete( EHostType hostType, VxGUID& sessionId ) override;

    virtual void				callbackGuiGroupieListSearchResult( GroupieId& groupieId, GuiGroupie* guiGroupie, VxGUID& sessionId ) override;
    virtual void				callbackGuiGroupieListSearchComplete( EHostType hostType, VxGUID& sessionId ) override;

    virtual void				callbackGuiHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    virtual void				callbackGuiHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    virtual void				callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    virtual void				callbackGuiHostUnJoinGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;

    virtual void				callbackGuiHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    virtual void				callbackGuiHostJoinLeaveHost( GroupieId& groupieId ) override;
    virtual void				callbackGuiHostUnJoin( GroupieId& groupieId ) override;
    virtual void				callbackGuiHostJoinRemoved( GroupieId& groupieId ) override;

    virtual void				callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    virtual void				callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    virtual void				callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    virtual void				callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;

    virtual void				callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    virtual void				callbackGuiUserJoinLeaveHost( GroupieId& groupieId ) override;
    virtual void				callbackGuiUserJoinRemoved( GroupieId& groupieId ) override;

protected slots:
    void                        slotChooseHostModeButtonClick( void );
    void                        slotShowIgnoredHostsListButtonClicked( void );

    void                        slotIconButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotMenuButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotJoinButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotConnectButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotKickButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotIgnoreButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );

protected:
    void                        changeGuiMode( bool userListMode );
    virtual void				queryHostedList( void );

    void                        updateUser( GuiUser* guiUser );

    void                        onJoinedHost( GroupieId& groupieId, GuiHosted* guiHosted );

    bool                        launchClientApplet( GuiHosted* guiHosted );

protected:
    Ui::AppletJoinHostListUi    ui;
    VxGUID                      m_JoinedHostSession;
    bool                        m_UserListMode{ false };
    GroupieId                   m_AdminGroupieId;

	std::string					m_NetworkHostUrl;
	VxPtopUrl					m_NetHostPtopUrl;
	VxGUID                      m_NetworkHostOnlineId;

	EHostType                   m_HostType{ eHostTypeUnknown };
};


