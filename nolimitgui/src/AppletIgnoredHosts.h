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

#include "AppletJoinBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletIgnoredHostsUi;
}
QT_END_NAMESPACE

#include <IdentListMgrs/IgnoreListMgr.h>

class GuiUserListItem;

class AppletIgnoredHosts : public AppletBase
{
	Q_OBJECT
public:
    AppletIgnoredHosts( AppCommon& app, QWidget* parent );
	virtual ~AppletIgnoredHosts();

    virtual void                setStatusMsg( QString statusMsg );
    virtual void                setListLabel( QString labelText );


protected slots:
    void                        slotIconButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotMenuButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotJoinButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotConnectButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotKickButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );
    void                        slotIgnoreButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem );

protected:
    void                        queryIgnoredHostsList( void );
    void                        insertIntoHostList( IgnoredHostInfo& hostInfo );

protected:
    Ui::AppletIgnoredHostsUi&   ui;
    std::map<VxGUID, IgnoredHostInfo> m_IgnoredHostList;
};


