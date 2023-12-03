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

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <GuiInterface/IDefs.h>

class VxGUID;
class VxNetIdent;

class ToGuiUserUpdateInterface
{
public:
    virtual void				toGuiIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) = 0;
    virtual void				toGuiIndentListRemove( EUserViewType listType, VxGUID& onlineId ) = 0;

    virtual void				toGuiContactAdded( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiContactRemoved( VxGUID& onlineId ) = 0; 

    virtual void				toGuiContactNameChange( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiContactDescChange( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiContactMyFriendshipChange( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiContactHisFriendshipChange( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiPluginPermissionChange( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiContactSearchFlagsChange( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiContactLastSessionTimeChange( VxNetIdent* netIdent ) = 0; 

    virtual void				toGuiUpdateMyIdent( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiSaveMyIdent( VxNetIdent* netIdent ) = 0;

    virtual void				toGuiPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus ) = 0;
};


