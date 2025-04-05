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

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletHostSelectUi;
}
QT_END_NAMESPACE

class GuiHostJoin;
class GuiHostedByMeJoinMgr;
class GroupieId;
class GuiHostJoinSession;
class HostJoinRequestListItem;

class AppletHostSelect : public AppletBase
{
	Q_OBJECT
public:
	AppletHostSelect( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletHostSelect() override;

signals:
	void                        signalGroupHostSelected( QString hostUrl );

protected slots:
	void				        slotHostJoinRequested( GuiHostJoin* guiHostJoin );
	void				        slotlHostJoinUpdated( GuiHostJoin* guiHostJoin );
	void				        slotHostJoinRemoved( GroupieId groupieId );
	void                        slotHostJoinOfferStateChange( GroupieId groupieId, EJoinState hostOfferState );
	void                        slotHostJoinOnlineStatus( GuiHostJoin* user, bool isOnline );

	void                        slotAcceptButtonClicked( GuiHostJoinSession* joinSession, HostJoinRequestListItem* joinItem );
	void                        slotRejectButtonClicked( GuiHostJoinSession* joinSession, HostJoinRequestListItem* joinItem );

	void                        slotJoinComboBoxSelectionChange( int comboIdx );
	void                        slotCreateInviteButtonClicked( void );

protected:
	void						updateJoinList( void );
	void						updateHostJoinRequest( GuiHostJoin* guiHostJoin );

	//=== vars ===//
	Ui::AppletHostSelectUi&		ui;
	GuiHostedByMeJoinMgr&		m_HostJoinMgr;
	EHostType					m_HostType{ eHostTypeGroup };
	EJoinState					m_JoinState{ eJoinStateJoinRequested };
};
