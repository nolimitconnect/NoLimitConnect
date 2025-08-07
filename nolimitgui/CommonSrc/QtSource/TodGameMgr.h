#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget>

#include <GuiInterface/IDefs.h>

class VxGUID;

class TodGameCallback
{
public:
    virtual void				toGuiTodGameAction( EPluginType	pluginType, VxGUID& onlineId, ETodGameAction todGameAction ) = 0;
};


class TodGameMgr
{
public:
	TodGameMgr() = default;

    void                        wantTodGamCallbacks( TodGameCallback* clientInterface, bool wantCallbacks );


    void				        toGuiTodGameAction( EPluginType	pluginType, VxGUID& onlineId, ETodGameAction todGameAction );


protected:


    //=== vars ===//
    std::vector<TodGameCallback*> m_TodClients;
};
