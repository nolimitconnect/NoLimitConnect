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

#include "GuiUserUpdateCallback.h"
#include "GuiHostedListCallback.h"
#include "GuiGroupieListCallback.h"
#include "GuiHostJoinCallback.h"
#include "GuiUserJoinCallback.h"

#include <CoreLib/VxPtopUrl.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletJoinHostListUi;
}
QT_END_NAMESPACE

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

    void                        setStatusMsg( QString statusMsg );
    void                        setListLabel( QString labelText );

    void				        callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )  override;

    void				        callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override {};
    void				        callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId ) override {};
    void				        callbackUserAdded( GuiUser* guiUser ) override;
    void				        callbackUserUpdated( GuiUser* guiUser ) override;
    void				        callbackUserRemoved( VxGUID& onlineId ) override {};

    void				        callbackGuiHostedListSearchResult( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId ) override;
    void				        callbackGuiHostedListSearchStatus( EHostType hostType, VxGUID& sessionId, EConnectStatus connectStatus ) override;
    void				        callbackGuiHostedListSearchComplete( EHostType hostType, VxGUID& sessionId ) override;

    void				        callbackGuiGroupieListSearchResult( GroupieId& groupieId, GuiGroupie* guiGroupie, VxGUID& sessionId ) override;
    void				        callbackGuiGroupieListSearchComplete( EHostType hostType, VxGUID& sessionId ) override;

    void				        callbackGuiHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    void				        callbackGuiHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    void				        callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    void				        callbackGuiHostUnJoinGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;

    void				        callbackGuiHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    void				        callbackGuiHostJoinLeaveHost( GroupieId& groupieId ) override;
    void				        callbackGuiHostUnJoin( GroupieId& groupieId ) override;
    void				        callbackGuiHostJoinRemoved( GroupieId& groupieId ) override;

    void				        callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;

    void				        callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinLeaveHost( GroupieId& groupieId ) override;
    void				        callbackGuiUserJoinRemoved( GroupieId& groupieId ) override;

    void				        callbackGuiUserJoinAHostStatus( EHostType hostType, VxGUID& sessionId, EConnectStatus connectStatus ) override;

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
    Ui::AppletJoinHostListUi&   ui;
    VxGUID                      m_JoinedHostSession;
    bool                        m_UserListMode{ false };
    GroupieId                   m_AdminGroupieId;

	std::string					m_NetworkHostUrl;
	VxPtopUrl					m_NetHostPtopUrl;
	VxGUID                      m_NetworkHostOnlineId;

	EHostType                   m_HostType{ eHostTypeUnknown };
    EConnectStatus              m_ConnectStatus{ eConnectStatusUnknown };
};


