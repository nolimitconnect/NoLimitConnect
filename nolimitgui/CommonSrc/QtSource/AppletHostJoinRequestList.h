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

#include "AppletBase.h"
#include "ui_AppletHostJoinRequestList.h"

class GuiHostJoin;
class GuiHostJoinMgr;
class GroupieId;

class AppletHostJoinRequestList : public AppletBase
{
	Q_OBJECT
public:
	AppletHostJoinRequestList( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletHostJoinRequestList() override;

protected slots:
	void				        slotHostJoinRequested( GuiHostJoin* user );
	void				        slotlHostJoinUpdated( GuiHostJoin* user );
	void				        slotHostJoinRemoved( GroupieId& groupieId );
	void                        slotHostJoinOfferStateChange( GroupieId& groupieId, EJoinState hostOfferState );
	void                        slotHostJoinOnlineStatus( GuiHostJoin* guiHostJoin, bool isOnline );

	void                        slotAcceptButtonClicked( GuiHostJoinSession* joinSession, HostJoinRequestListItem* joinItem );
	void                        slotRejectButtonClicked( GuiHostJoinSession* joinSession, HostJoinRequestListItem* joinItem );

	void                        slotJoinComboBoxSelectionChange( int comboIdx );
	void                        slotInviteCreateButtonClicked( void );
	void                        slotInviteAcceptButtonClicked( void );

protected:
	void						updateJoinList( void );
	void						updateHostJoinRequest( GuiHostJoin* guiHostJoin );

	//=== vars ===//
	Ui::AppletHostJoinRequestListUi		ui;
	GuiHostJoinMgr&				m_HostJoinMgr;
	EHostType					m_HostType{ eHostTypeGroup };
	EJoinState					m_JoinState{ eJoinStateJoinRequested };
};
