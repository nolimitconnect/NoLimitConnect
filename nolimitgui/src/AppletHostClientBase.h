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

#include "AppletClientBase.h"

#include "CanSendInterface.h"
#include "InputClientBaseCallback.h"

#include <GuiInterface/IDefs.h>
#include <GuiInterface/IToGui.h> 

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletHostClientUi;
}
QT_END_NAMESPACE

class CalendarEventListPanel;
class VxPushButton;

class AppletHostClientBase : public AppletClientBase, public InputClientBaseCallback, public CanSendInterface
{
    Q_OBJECT
public:
	AppletHostClientBase( const char* objName, AppCommon& app, EApplet applet, EHostType hostType, EPluginType pluginType, QWidget* parent );
	virtual ~AppletHostClientBase() override;

    AppCommon&                  getMyApp( void ) override { return m_MyApp; }
    EPluginType			        getInputClientPluginType( void ) override { return AppletHostBase::getPluginType(); }

    void				        userJoinedHost( GuiHosted* guiHosted ) override;
    void                        setAdminGroupieId( GroupieId& adminGroupieId ) override;

protected slots:
    void                        slotSetSessionVisible( bool makeVisible );
    void                        slotSetMembersVisible( bool makeVisible );

    void						slotViewChanged( EUserViewType viewType );

    void                        slotUserSelected( GuiUser* guiUser );

    void                        slotSendingToMember( VxGUID assetId, VxGUID memberId, QString memberName );
    void                        slotMultiSendComplete( VxGUID assetId, bool allSucceeded, int successCount, int failCount );

    void                        slotCalendarToggleClicked( bool checked );
    void                        slotCalendarEventListUpdated( EHostType hostType, VxGUID hostOnlineId );

protected:
    //! restyles m_CalendarToggleButton to flag unviewed events -- see GuiCalendarMgr::
    //! hasUnviewedEvents. no per-identity "viewed" record leaves this device -- see
    //! event-calendar design notes, "no per-identity RSVP record".
    void                        updateCalendarUnreadIndicator( void );
    void                        showEvent( QShowEvent* ev ) override;

    GroupieId                   getActiveAdminGroupieId( void ); // original admin groupie id for this applet session
    GroupieId                   getSendToAdminGroupieId( void ); // same as original but may have selected user to send to

    bool						checkIfCanSend( void ) override;
    bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;
    ECanSendState               getCanSendState( void ) override;

    //=== vars ===//
    Ui::AppletHostClientUi&	    ui;
    GroupieId                   m_AdminGroupieId;
    GroupieId                   m_SendToGroupieAdminId;
    GuiUser*                    m_SelectedUser{nullptr};

    //! event calendar for this joined host -- collapsed ( hidden ) by default, revealed by
    //! m_CalendarToggleButton. see event-calendar design notes, "Event session GUI".
    VxPushButton*               m_CalendarToggleButton{ nullptr };
    CalendarEventListPanel*     m_CalendarPanel{ nullptr };
};


