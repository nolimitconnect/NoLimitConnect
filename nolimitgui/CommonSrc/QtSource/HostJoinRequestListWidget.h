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

#include "ListWidgetBase.h"

class HostJoinRequestListItem;
class GuiHostJoin;
class GuiHostJoinSession;
class GroupieId;

class HostJoinRequestListWidget : public ListWidgetBase
{
	Q_OBJECT

public:
	HostJoinRequestListWidget( QWidget* parent );

    void                        clearHostJoinRequestList( void );

    void                        addOrUpdateHostRequest( GuiHostJoin* hostJoin );
    HostJoinRequestListItem*    addOrUpdateHostRequestSession( GuiHostJoinSession* hostSession );

    HostJoinRequestListItem*    findListEntryWidgetByGroupieId( GroupieId& groupieId );
    void                        removeHostJoinRequest( GroupieId& groupieId );

signals:
    void                        signalAvatarButtonClicked( GuiHostJoinSession* hostSession, HostJoinRequestListItem* hostItem );
    void                        signalFrienshipButtonClicked( GuiHostJoinSession* hostSession, HostJoinRequestListItem* hostItem );
    void                        signalMenuButtonClicked( GuiHostJoinSession* hostSession, HostJoinRequestListItem* hostItem );
    void                        signalAcceptButtonClicked( GuiHostJoinSession* hostSession, HostJoinRequestListItem* hostItem );
    void                        signalRejectButtonClicked( GuiHostJoinSession* hostSession, HostJoinRequestListItem* hostItem );

protected slots:
	void						slotItemClicked( QListWidgetItem* item );
    void                        slotHostJoinRequestListItemClicked( QListWidgetItem* hostItem );
    void                        slotAvatarButtonClicked( HostJoinRequestListItem* hostItem );
    void                        slotlFriendshipButtonClicked( HostJoinRequestListItem* hostItem );
    void                        slotMenuButtonClicked( HostJoinRequestListItem* hostItem );
    void                        slotAcceptButtonClicked( HostJoinRequestListItem* hostItem );
    void                        slotRejectButtonClicked( HostJoinRequestListItem* hostItem );

protected:
    HostJoinRequestListItem*    sessionToWidget( GuiHostJoinSession* hostSession );
    GuiHostJoinSession*			widgetToSession( HostJoinRequestListItem* hostItem );

    virtual void                onHostJoinRequestListItemClicked( HostJoinRequestListItem* hostItem );
    virtual void                onAvatarButtonClicked( HostJoinRequestListItem* hostItem );
    virtual void                onFriendshipButtonClicked( HostJoinRequestListItem* hostItem );
    virtual void                onMenuButtonClicked( HostJoinRequestListItem* hostItem );
    virtual void                onAcceptButtonClicked( HostJoinRequestListItem* hostItem );
    virtual void                onRejectButtonClicked( HostJoinRequestListItem* hostItem );


	//=== vars ===//

};

