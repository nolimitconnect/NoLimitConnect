#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones 
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

#include <vector>
#include <memory>

class VxSktBase;

class IConnectedCallback
{
public:
    /// return true if have use for this connection
    virtual bool                onContactConnected( std::shared_ptr<VxSktBase>& sktBase ) = 0;
    virtual void                onContactDisconnected( std::shared_ptr<VxSktBase>& sktBase ) = 0;
};
