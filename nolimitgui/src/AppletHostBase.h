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

#include <GuiInterface/IToGui.h>
#include "GuiUserUpdateCallback.h"

class GuiHosted;
class GuiHostSession;
class GuiUserListWidget;
class GuiUserMultiListWidget;

class AppletHostBase : public AppletBase, public GuiUserUpdateCallback
{
	Q_OBJECT
public:
    AppletHostBase( const char* objName, AppCommon& app, QWidget* parent );
	virtual ~AppletHostBase();

    void                        setHostType( enum EHostType hostType )      { m_HostType = hostType; }
    EHostType                   getHostType(  void )                        { return m_HostType; }

    void                        setSearchType( ESearchType searchType )     { m_SearchType = searchType; }
    ESearchType                 getSearchType(  void )                      { return m_SearchType; }

    virtual void				manageUsers( GuiUserMultiListWidget* userList );
    virtual void				manageUsers( GuiUserListWidget* userList );
    virtual void				manageHostSession( GuiHostSession* hostSession, bool requestJoin );

    virtual void				userJoinedHost( GuiHosted* guiHosted );

    virtual void                updateUser( GuiUser* guiUser );
    virtual void                removeUser( VxGUID& onlineId );

    void                        wantUserUpdateCallbacks( bool enable );

protected:
    virtual void				callbackIndentListUpdate( enum EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override;
    virtual void				callbackIndentListRemove( enum EUserViewType listType, VxGUID& onlineId ) override;

    virtual void				callbackUserAdded( GuiUser* guiUser ) override;
    virtual void				callbackUserUpdated( GuiUser* guiUser ) override;
    virtual void				callbackUserRemoved( VxGUID& onlineId ) override;

    //=== vars ===//
    EHostType                   m_HostType{ eHostTypeUnknown };
    ESearchType                 m_SearchType{ eSearchNone };
    GuiUserMultiListWidget*     m_UserMultiList{ nullptr };
    GuiUserListWidget*          m_UserList{ nullptr };
    std::string                 m_HostUrl;
    VxGUID                      m_HostSessionId;
    bool                        m_UserUpdateCallbacksRequested{ false };

    GuiHosted*                  m_LastGuiHosted{ nullptr };
};


