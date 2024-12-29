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

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include <GuiInterface/IDefs.h>
#include <stdint.h>

class VxGUID;
class VxNetIdent;

class ToGuiUserUpdateInterface
{
public:
    virtual void				toGuiIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) = 0;
    virtual void				toGuiIndentListRemove( EUserViewType listType, VxGUID& onlineId ) = 0;

    virtual void				toGuiContactAdded( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiContactRemoved( VxGUID& onlineId ) = 0; 

     virtual void				toGuiContactUpdated( VxNetIdent* netIdent ) = 0; 
        ; 
    virtual void				toGuiContactLastSessionTimeChange( VxNetIdent* netIdent ) = 0; 

    virtual void				toGuiUpdateMyIdent( VxNetIdent* netIdent ) = 0; 
    virtual void				toGuiSaveMyIdent( VxNetIdent* netIdent ) = 0;
};


