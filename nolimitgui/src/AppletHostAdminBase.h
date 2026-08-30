#pragma once
//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include "InputClientBaseCallback.h"

#include <CoreLib/HostedId.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletHostClientUi;
}
QT_END_NAMESPACE

class CalendarEventListPanel;
class VxPushButton;

class AppletHostAdminBase : public AppletBase, public InputClientBaseCallback
{
	Q_OBJECT
public:
    AppletHostAdminBase( const char* ObjName, AppCommon& app, QWidget* parent );
	virtual ~AppletHostAdminBase() override;

    AppCommon&                  getMyApp( void ) override { return m_MyApp; }
    EPluginType			        getInputClientPluginType( void ) override { return ePluginTypeClientGroup; }

    //! called by each leaf class ( AppletChatRoomHostAdmin/AppletGroupHostAdmin/
    //! AppletRandomConnectHostAdmin ) right after they build their own hostAdminId -- this IS
    //! always the admin of their own host ( no permission check needed, unlike the client-side
    //! CalendarEventListPanel::setHostAdminId call in AppletHostClientBase ). see event-calendar
    //! design notes -- this is the natural place a host admin looks to create/manage events,
    //! separate from the client-session placement in AppletHostClientBase ( Phase 4f ).
    void                        setCalendarHostedId( HostedId& hostedId );

protected:
protected slots:
    void                        slotSetMembersVisible( bool visible );
    void                        slotSetSessionVisible( bool visible );
    void                        slotCalendarToggleClicked( bool checked );
    void                        slotCalendarEventListUpdated( EHostType hostType, VxGUID hostOnlineId );

    void                        slotUserSelected( GuiUser* guiUser );

    void                        slotSendingToMember( VxGUID assetId, VxGUID memberId, QString memberName );
    void                        slotMultiSendComplete( VxGUID assetId, bool allSucceeded, int successCount, int failCount );

protected:
    bool						checkIfCanSend( void ) override;
    bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;

    //! restyles m_CalendarToggleButton to flag unviewed events -- see GuiCalendarMgr::
    //! hasUnviewedEvents. no per-identity "viewed" record leaves this device -- see
    //! event-calendar design notes, "no per-identity RSVP record".
    void                        updateCalendarUnreadIndicator( void );

    //=== vars ===//
    Ui::AppletHostClientUi&     ui;
    GuiUser*                    m_SelectedUser{nullptr};

    //! collapsed ( hidden ) by default, revealed by m_CalendarToggleButton -- same convention
    //! as AppletHostClientBase's own calendar toggle ( Phase 4f ).
    VxPushButton*               m_CalendarToggleButton{ nullptr };
    CalendarEventListPanel*     m_CalendarPanel{ nullptr };
    HostedId                    m_CalendarHostedId;
};


