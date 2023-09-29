#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
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
