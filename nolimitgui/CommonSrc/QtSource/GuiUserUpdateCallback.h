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

#include <GuiInterface/IDefs.h>

class VxGUID;
class GuiUser;

class GuiUserUpdateCallback
{
public:
    virtual void				callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) {};
    virtual void				callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId ) {};

    virtual void				callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) {};

    virtual void				callbackUserAdded( GuiUser* guiUser ) {};
    virtual void				callbackUserUpdated( GuiUser* guiUser ) {};
    virtual void				callbackUserRemoved( VxGUID& onlineId ) {};

    virtual void				callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus ) {};

    virtual void				callbackMyIdentUpdated( GuiUser* guiUser ) {};
};