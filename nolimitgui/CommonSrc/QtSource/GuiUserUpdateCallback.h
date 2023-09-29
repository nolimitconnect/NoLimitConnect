#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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