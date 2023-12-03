#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//



//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <typeinfo>
#include <inttypes.h>

class AssetBaseInfo;

class IMediaPlayerRequests
{
public:
    virtual void				fromGuiInitCommandLine( int argc, char** arg ) = 0;
    virtual void				fromStartModule( EAppModule appModule ) = 0;
    virtual void				fromStopModule( EAppModule appModule ) = 0;

    virtual bool				fromGuiPlayMedia( AssetBaseInfo& assetInfo, int pos0to100000 ) = 0;
    virtual bool				fromGuiMediaPlayerAction( EMediaPlayerAction playerAction ) = 0;
    virtual bool				fromGuiMediaPlayerSeek( int position0to100000 ) = 0;
};
