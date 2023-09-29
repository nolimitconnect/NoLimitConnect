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
#pragma once

#include <CoreLib/VxGUID.h>
#include <CoreLib/VxMutex.h>

#include <map>

class ConnectedInfo;
class VxSktBase;

class ConnectedListBase
{
public:
    ConnectedListBase() = default;
    virtual ~ConnectedListBase() = default;

    void                        lockConnectedList( void )   { m_ConnectedListMutex.lock(); }
    void                        unlockConnectedList( void ) { m_ConnectedListMutex.unlock(); }

    std::map<std::pair<VxGUID, VxGUID>, ConnectedInfo *>&	getConnectedList( void )		{ return m_ConnectList; }

protected:
    VxMutex                     m_ConnectedListMutex;
    std::map<std::pair<VxGUID, VxGUID>, ConnectedInfo *>    m_ConnectList; // pair is socketId, onlineId
};
